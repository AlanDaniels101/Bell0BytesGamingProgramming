#pragma once

#pragma region "Description"

/******************************************************************************************************************************* 
* Implementation of Expected<T> 
*
* Expect<T> is either the type T, or it is the exception that prevented the creation of T.
*
* Original implementation of Expected<T> by Gilles Bellot from bell0bytes game programming tutorial
* - https://bell0bytes.eu/expected/
*
* This is from Andrei Alexandrescu's talk: C++ and Beyond 2012: Andrei Alexandrescu - Systematic Error Handling in C++
* - https://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C
*
********************************************************************************************************************************/

#pragma endregion

#pragma region "Includes"

// Exception handling
#include <exception>
#include <stdexcept>

// Atomic
#include <atomic>

#pragma endregion

namespace util
{
	// Expected template for generic type T
	template<class T>
	class Expected
	{
		union
		{
			T result;
			std::exception_ptr exception;
		};

		bool isResultValid;
		Expected() {};

	public:
		// Create Expected<T> from valid T
		Expected(const T& r) : result(r), isResultValid(true) {}
		Expected(T&& r) : result(std::move(r)), isResultValid(true) {}
		// Create Expected<T> from other Expected<T>
		Expected(const Expected& other) : isResultValid(other.isResultValid)
		{
			if (isResultValid)
				new(&result) T(other.result);
			else
				new(&exception) std::exception_ptr(other.exception);
		}
		Expected(Expected&& other) : isResultValid(other.isResultValid)
		{
			if (isResultValid)
				new(&result) T(std::move(other.result));
			else
				new(&exception) std::exception_ptr(std::move(other.exception));
		}
		~Expected()
		{
			using std::exception_ptr;
			if (isResultValid)
				result.~T();
			else
				exception.~exception_ptr();
		}

		// Create Expected<T> from exception E
		template<class E>
		static Expected<T> fromException(const E& e)
		{
			if (typeid(e) != typeid(E))
				throw std::invalid_argument("Slicing detected!");
			return fromException(std::make_exception_ptr(e));
		}
		static Expected<T> fromException(std::exception_ptr p)
		{
			Expected<T> newExpected;
			newExpected.isResultValid = false;
			new(&newExpected.exception) std::exception_ptr(std::move(p));
			return newExpected;
		}
		static Expected<T> fromException()
		{
			return fromException(std::current_exception());
		}

		void swap(Expected& other)
		{
			if (isResultValid)
			{
				if (other.isResultValid)
				{
					std::swap(result, other.result);
				}
				else
				{
					auto t = std::move(other.exception);
					new(&other.result) T(std::move(result));
					new(&exception) std::exception_ptr(t);
					std::swap(isResultValid, other.isResultValid);
				}
			}
			else
			{
				if (other.isResultValid)
				{
					other.swap(*this);
				}
				else
				{
					exception.swap(other.exception);
					std::swap(isResultValid, other.isResultValid);
				}
			}
		}

		Expected<T>& operator=(const Expected<T>& other)
		{
			isResultValid = other.isResultValid;
			if (isResultValid)
				new(&result) T(other.result);
			else
				new(&exception) std::exception_ptr(other.exception);
			return *this;
		}

		bool isValid() const { return isResultValid; };

		T& get()
		{
			if (!isResultValid)
				std::rethrow_exception(exception);
			return result;
		}
		const T& get() const
		{
			if (isResultValid)
				std::rethrow_exception(exception);
			return result;
		}

		template<class E>
		bool hasException() const
		{
			try
			{
				if (!isResultValid)
					std::rethrow_exception(exception);
			}
			catch (const E& object)
			{
				(void)object;
				return true;
			}
			catch (...)
			{
				
			}
			return false;
		}

		friend class Expected<void>;
	};

	// Special Expected for void type
	template<>
	class Expected<void>
	{
		std::exception_ptr exception;

	public:
		template<typename E>
		Expected(E const& e) : exception(std::make_exception_ptr(e)) {}
		template<typename T>
		Expected(const Expected<T>& other)
		{
			if (!other.isResultValid)
				new(&exception) std::exception_ptr(other.exception);
		}

		Expected(Expected&& other) : exception(std::move(other.exception)) {}
		Expected() : exception() {}

		Expected<void>& operator=(const Expected<void>& other)
		{
			if (!other.isValid())
				this->exception = other.exception;
			return *this;
		};

		bool isValid() const { return !exception; }
		void get() const 
		{
			if (!isValid())
				std::rethrow_exception(exception);
		}
		void suppress() {}

	};
}