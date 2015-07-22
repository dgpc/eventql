/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_THREAD_FIXEDSIZETHREADPOOL_H
#define _FNORDMETRIC_THREAD_FIXEDSIZETHREADPOOL_H
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <thread>
#include "fnord/thread/task.h"
#include "fnord/thread/queue.h"
#include "fnord/thread/taskscheduler.h"
#include "fnord/thread/wakeup.h"
#include "fnord/exceptionhandler.h"

namespace fnord {
namespace thread {

/**
 * A threadpool is threadsafe
 */
class FixedSizeThreadPool : public TaskScheduler {
public:

  /**
   * Construct a new fixed size thread pool. If maxqueuelen is set, the run()
   * operation will block if the queue is full. Default is an unbounded queue
   *
   * @param nthreads number of threads to run
   * @param maxqueuelen max queue len. default is -1 == unbounded
   */
  FixedSizeThreadPool(
      size_t nthreads,
      size_t maxqueuelen = -1);

  /**
   * Construct a new fixed size thread pool. If maxqueuelen is set, the run()
   * operation will block if the queue is full. Default is an unbounded queue
   *
   * @param nthreads number of threads to run
   * @param error_handler the exception handler to call for unhandled errors
   * @param maxqueuelen max queue len. default is -1 == unbounded
   */
  FixedSizeThreadPool(
      size_t nthreads,
      std::unique_ptr<fnord::ExceptionHandler> error_handler,
      size_t maxqueuelen = -1);

  void start();
  void stop();

  void run(std::function<void()> task) override;
  void runOnReadable(std::function<void()> task, int fd) override;
  void runOnWritable(std::function<void()> task, int fd) override;
  void runOnWakeup(
      std::function<void()> task,
      Wakeup* wakeup,
      long generation) override;

protected:

  size_t nthreads_;
  std::unique_ptr<fnord::ExceptionHandler> error_handler_;
  Queue<std::function<void()>> queue_;
  Vector<std::thread> threads_;
};

}
}
#endif
