/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gloo_collective_group.h"

#include <gloo/algorithm.h>
#include <gloo/allgather.h>
#include <gloo/barrier.h>
#include <gloo/broadcast.h>
#include <gloo/reduce.h>
#include <gloo/rendezvous/prefix_store.h>
#include <gloo/scatter.h>
#include <gloo/transport/tcp/device.h>

#include "api/cpp/src/utils/utils.h"
#include "yr/api/kv_manager.h"

namespace YR::collective {

#define EXECUTE_BY_TYPE(dType, OPERATION, ...)                                \
    switch (dType) {                                                          \
        case DataType::INT:                                                   \
            OPERATION<int>(__VA_ARGS__);                                      \
            break;                                                            \
        case DataType::DOUBLE:                                                \
            OPERATION<double>(__VA_ARGS__);                                   \
            break;                                                            \
        case DataType::INVALID:                                               \
        default:                                                              \
            throw YR::Exception(YR::Libruntime::ErrorCode::ERR_PARAM_INVALID, \
                                "invalid dType: " + std::to_string(dType));   \
    }

struct DsStore : public gloo::rendezvous::Store {
public:
    DsStore() = default;
    ~DsStore() override = default;

    void set(const std::string &key, const std::vector<char> &data) override
    {
        YR::KVManager::Set(key, std::string(data.begin(), data.end()));
    }

    std::vector<char> get(const std::string &key) override
    {
        std::vector<char> out;
        std::string result = YR::KVManager::Get(key);
        out.assign(result.begin(), result.end());
        return out;
    }

    void wait(const std::vector<std::string> &keys) override
    {
        YR::KVManager::Get(keys);
    }

    void wait(const std::vector<std::string> &keys, const std::chrono::milliseconds &timeout) override
    {
        YR::KVManager::Get(keys, static_cast<int>(timeout.count() / 1000));
    }
};

GlooCollectiveGroup::GlooCollectiveGroup(std::string groupName, int worldSize, int rank)
    : CollectiveGroup(std::move(groupName), worldSize, rank, Backend::GLOO)
{
    auto dsStore = std::make_shared<DsStore>();
    auto prefixStore = std::make_shared<gloo::rendezvous::PrefixStore>(groupName_, dsStore);

    gloo::transport::tcp::attr attr;
    attr.hostname = "";  // todo 从环境变量里取本地地址 也支持环境变量配置使用哪种后端
    auto dev = gloo::transport::tcp::CreateDevice(attr);

    context_ = std::make_shared<gloo::rendezvous::Context>(rank_, worldSize_);
    context_->connectFullMesh(prefixStore, dev);
}

void GlooCollectiveGroup::AllReduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op)
{
    EXECUTE_BY_TYPE(input.dType, DoAllReduce, input, output, op);
}

void GlooCollectiveGroup::Reduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op, int dstRank)
{
    EXECUTE_BY_TYPE(input.dType, DoReduce, input, output, op, dstRank);
}

void GlooCollectiveGroup::AllGather(const DataDescriptor &input, DataDescriptor &output)
{
    EXECUTE_BY_TYPE(input.dType, DoAllGather, input, output);
}

void GlooCollectiveGroup::Barrier()
{
    gloo::BarrierOptions opts(context_);
    gloo::barrier(opts);
}

void GlooCollectiveGroup::Scatter(const DataDescriptor &input, DataDescriptor &output, int srcRank)
{
    EXECUTE_BY_TYPE(input.dType, DoScatter, input, output, srcRank);
}

void GlooCollectiveGroup::Broadcast(const DataDescriptor &input, DataDescriptor &output, int srcRank)
{
    EXECUTE_BY_TYPE(input.dType, DoBroadcast, input, output, srcRank);
}

void GlooCollectiveGroup::Recv(DataDescriptor &output, int srcRank, int tag)
{
    auto ubuf = context_->createUnboundBuffer(output.buf, output.count);
    ubuf->recv(srcRank, tag);
    ubuf->waitRecv();
}

void GlooCollectiveGroup::Send(const DataDescriptor &input, int dstRank, int tag)
{
    auto ubuf = context_->createUnboundBuffer(input.buf, input.count);
    ubuf->send(dstRank, tag);
    ubuf->waitSend();
}

template <typename T>
gloo::AllreduceOptions::Func GlooCollectiveGroup::GetReduceOp(const ReduceOp &op)
{
    void (*fn)(void *, const void *, const void *, long unsigned int) = &gloo::sum<T>;
    switch (op) {
        case ReduceOp::SUM:
            fn = &gloo::sum<T>;
            break;
        case ReduceOp::PRODUCT:
            fn = &gloo::product<T>;
            break;
        case ReduceOp::MIN:
            fn = &gloo::min<T>;
            break;
        case ReduceOp::MAX:
            fn = &gloo::max<T>;
            break;
    }
    return fn;
}

template <typename T>
void GlooCollectiveGroup::DoAllReduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op)
{
    gloo::AllreduceOptions opts(context_);
    opts.setInput(static_cast<T *>(input.buf), input.count);
    opts.setOutput(static_cast<T *>(output.buf), output.count);
    opts.setReduceFunction(GetReduceOp<T>(op));
    gloo::allreduce(opts);
}

template <typename T>
void GlooCollectiveGroup::DoReduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op, int dstRank)
{
    gloo::ReduceOptions opts(context_);
    opts.setInput(static_cast<T *>(input.buf), input.count);
    opts.setOutput(static_cast<T *>(output.buf), output.count);
    opts.setReduceFunction(GetReduceOp<T>(op));
    gloo::reduce(opts);
}

template <typename T>
void GlooCollectiveGroup::DoBroadcast(const DataDescriptor &input, DataDescriptor &output, int srcRank)
{
    gloo::BroadcastOptions opts(context_);
    opts.setInput(static_cast<T *>(input.buf), input.count);
    opts.setOutput(static_cast<T *>(output.buf), output.count);
    opts.setRoot(srcRank);
    gloo::broadcast(opts);
}

template <typename T>
void GlooCollectiveGroup::DoScatter(const DataDescriptor &input, DataDescriptor &output, int srcRank)
{
    gloo::ScatterOptions opts(context_);
    std::vector<T *> inputs = {static_cast<T *>(input.buf)};
    opts.setInputs(inputs, input.count);
    opts.setOutput(static_cast<T *>(output.buf), output.count);
    opts.setRoot(srcRank);
    gloo::scatter(opts);
}

template <typename T>
void GlooCollectiveGroup::DoAllGather(const DataDescriptor &input, DataDescriptor &output)
{
    gloo::AllgatherOptions opts(context_);
    opts.setInput(static_cast<T *>(input.buf), input.count);
    opts.setOutput(static_cast<T *>(output.buf), output.count);
    gloo::allgather(opts);
}
}  // namespace YR::collective