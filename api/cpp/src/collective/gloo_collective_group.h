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

#pragma once
#include <gloo/allreduce.h>
#include <gloo/rendezvous/context.h>

#include "yr/collective/collective.h"

namespace YR::collective {
class GlooCollectiveGroup : public CollectiveGroup {
public:
    GlooCollectiveGroup(std::string groupName, int worldSize, int rank);

    void AllReduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op) override;

    void Reduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op, int dstRank) override;

    void AllGather(const DataDescriptor &input, DataDescriptor &output) override;

    void Barrier() override;

    void Scatter(const DataDescriptor &input, DataDescriptor &output, int srcRank) override;

    void Broadcast(const DataDescriptor &input, DataDescriptor &output, int srcRank) override;

    void Recv(DataDescriptor &output, int srcRank, int tag) override;

    void Send(const DataDescriptor &input, int dstRank, int tag) override;

private:
    template <typename T>
    void DoAllReduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op);

    template <typename T>
    void DoReduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op, int dstRank);

    template <typename T>
    void DoAllGather(const DataDescriptor &input, DataDescriptor &output);

    template <typename T>
    void DoScatter(const DataDescriptor &input, DataDescriptor &output, int srcRank);

    template <typename T>
    void DoBroadcast(const DataDescriptor &input, DataDescriptor &output, int srcRank);

    template <typename T>
    static gloo::AllreduceOptions::Func GetReduceOp(const ReduceOp &op);

    std::unordered_map<ReduceOp, gloo::AllreduceOptions::Func> map;

    std::shared_ptr<gloo::rendezvous::Context> context_;
};
}  // namespace YR::collective