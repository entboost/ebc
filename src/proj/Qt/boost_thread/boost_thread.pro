#-------------------------------------------------
#
# Project created by QtCreator 2017-08-13T19:30:05
#
#-------------------------------------------------

QT       -= core gui

TARGET = boost_thread
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        boost_thread.cpp

HEADERS += \
        boost_thread.h \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/detail/sync_deque_base.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/detail/sync_queue_base.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/deque_adaptor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/deque_base.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/deque_views.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/queue_adaptor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/queue_base.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/queue_op_status.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/queue_views.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/sync_bounded_queue.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/sync_deque.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/sync_priority_queue.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/sync_queue.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/concurrent_queues/sync_timed_queue.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/allocator_arg.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/allocator_traits.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/config.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/default_delete.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/pointer_traits.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/scoped_allocator.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/shared_ptr.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory/unique_ptr.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/deque.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/devector.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/functional.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/list.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/memory.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/queue.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/tuple.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/csbl/vector.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/config.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/counter.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/delete.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/force_cast.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/function_wrapper.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/invoke.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/invoker.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/is_convertible.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/lockable_wrapper.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/log.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/make_tuple_indices.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/memory.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/move.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/nullary_function.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/platform.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/singleton.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/thread.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/thread_group.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/thread_heap_alloc.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/thread_interruption.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/tss_hooks.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/variadic_footer.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/detail/variadic_header.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/detail/priority_executor_base.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/detail/scheduled_executor_base.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/basic_thread_pool.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/executor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/executor_adaptor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/generic_executor_ref.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/inline_executor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/loop_executor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/scheduled_thread_pool.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/scheduler.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/scheduling_adaptor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/serial_executor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/serial_executor_cont.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/thread_executor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executors/work.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/experimental/config/inline_namespace.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/experimental/parallel/v1/exception_list.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/experimental/parallel/v1/inline_namespace.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/experimental/parallel/v2/inline_namespace.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/experimental/parallel/v2/task_region.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/experimental/exception_list.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/experimental/task_region.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/futures/future_error.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/futures/future_error_code.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/futures/future_status.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/futures/is_future_type.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/futures/launch.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/futures/wait_for_all.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/futures/wait_for_any.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/condition_variable.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/condition_variable_fwd.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/once.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/once_atomic.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/pthread_mutex_scoped_lock.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/recursive_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/shared_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/shared_mutex_assert.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/thread_data.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/thread_heap_alloc.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/pthread/timespec.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/v2/shared_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/v2/thread.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/basic_recursive_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/basic_timed_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/condition_variable.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/interlocked_read.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/mfc_thread_init.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/once.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/recursive_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/shared_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/thread_data.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/thread_heap_alloc.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/win32/thread_primitives.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/barrier.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/caller_context.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/completion_latch.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/condition.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/condition_variable.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/cv_status.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/exceptional_ptr.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/exceptions.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/executor.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/externally_locked.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/externally_locked_stream.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/future.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/is_locked_by_this_thread.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/latch.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lock_algorithms.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lock_concepts.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lock_factories.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lock_guard.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lock_options.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lock_traits.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lock_types.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lockable_adapter.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lockable_concepts.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/lockable_traits.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/locks.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/null_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/once.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/ostream_buffer.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/poly_lockable.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/poly_lockable_adapter.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/poly_shared_lockable.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/poly_shared_lockable_adapter.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/recursive_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/reverse_lock.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/scoped_thread.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/shared_lock_guard.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/shared_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/strict_lock.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/sync_bounded_queue.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/sync_queue.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/synchronized_value.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/testable_mutex.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/thread.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/thread_functors.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/thread_guard.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/thread_only.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/thread_pool.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/thread_time.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/tss.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/user_scheduler.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/with_lock_guard.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread/xtime.hpp \
    F:/THIRDPARTY/boost_1_62_0/boost/thread.hpp
unix {
    target.path = /usr/lib
    INSTALLS += target
}
