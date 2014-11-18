#pragma once

#include <future>

namespace TMQ
{
	struct MessageBase
	{
		virtual ~MessageBase();
		MessageBase(std::size_t _uid, std::size_t _tid);
		std::size_t uid;
		std::size_t tid; // thread id
		MessageBase(const MessageBase&) = delete;
		MessageBase &operator=(const MessageBase&) = delete;
	protected:
		static std::size_t __shaderIdCounter;
	};

	template <typename T>
	struct Message : public MessageBase
	{
		T _data;

		virtual ~Message()
		{}

		static std::size_t getId()
		{
			static std::size_t id = MessageBase::__shaderIdCounter++;
			return id;
		}

		explicit Message(const T &data)
			: MessageBase(getId(), std::this_thread::get_id().hash()), _data(data)
		{}

		explicit Message(T &&data)
			: MessageBase(getId(), std::this_thread::get_id().hash()), _data(std::move(data))
		{}

		template <typename ...Args>
		explicit Message(Args ...args)
			: MessageBase(getId(), std::this_thread::get_id().hash()), _data(args...)
		{
		}

		Message &operator=(const Message&) = delete;
		Message(const Message&) = delete;
	};

	// Used for messages which return value
	// ex : struct MyReturnValueMsg : public FutureData<int>
	template <typename T>
	struct FutureData
	{
		virtual ~FutureData()
		{}

		std::promise<T> result;
		std::future<T> getFuture()
		{
			return result.get_future();
		}
		FutureData& operator=(const FutureData&) = delete;
		explicit FutureData(const FutureData&) = delete;
		explicit FutureData() = default;
		FutureData& operator=(FutureData&& o)
		{
			result = std::move(o.result);
			return *this;
		}
		explicit FutureData(FutureData&& o)
		{
			result = std::move(o.result);
		}
	};

	//TODO REMOVE
	class CloseQueue
	{
	};
}