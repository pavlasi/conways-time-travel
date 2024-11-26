#ifndef UTILS_HPP
#define UTILS_HPP

#include <optional>
#include <future>
#include <chrono>

namespace utils {

    /*
     *  launch_future()
     *  
     *  Launches an asynchronous task using std::async and returns a
     *  future to the result.  If the task cannot be launched due to a
     *  system error, it returns std::nullopt.
     *
     *  @func: A callable object (e.g., lambda, function, or functor)
     *  to be executed asynchronously.
     *
     *  return: 
     *    - An optional containing a std::future to the result of the
     *    callable.  If the task cannot be launched, returns
     *    std::nullopt.
     */
    template <class Callable>
    auto launch_future(Callable&& func) -> std::optional<std::future<std::invoke_result_t<Callable>>> {

        try {
            return std::async(
                std::launch::async,
                std::forward<Callable>(func)
            );

        } catch(std::system_error&) {
            return std::nullopt;
        }
    }

    /*
     *  wait_future()
     *
     *  Waits for a future to be ready within a specified timeout
     *  period.
     *
     *  @future: A reference to a std::future object containing the
     *  result of an asynchronous operation.
     *
     *  @time: A std::chrono duration specifying the maximum time to
     *  wait for the future.
     *
     *  return: 
     *    - An optional value of the future's result type. If the
     *    future becomes ready within the timeout period, returns the
     *    result contained in the future. Otherwise, returns
     *    std::nullopt.
     */ 
    template <class T, class Rep, class Period>
    std::optional<T> wait_future(std::future<T>& future, const std::chrono::duration<Rep, Period>& time) {

        if(time != std::chrono::duration<Rep, Period>::zero()) {
            if(future.wait_for(time) == std::future_status::ready) {
                return future.get();
            }
        }

        return std::nullopt;
    }
}

#endif  /* UTILS_HPP */
