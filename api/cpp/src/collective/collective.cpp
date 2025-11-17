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

#include "yr/collective/collective.h"

#include <json.hpp>
#include <ostream>

#include "api/cpp/src/utils/utils.h"
#include "gloo_collective_group.h"
#include "src/libruntime/err_type.h"
#include "src/utility/logger/logger.h"
#include "yr/api/exception.h"
#include "yr/api/kv_manager.h"

namespace YR::collective {

const std::string COLLECTIVE_GROUP_INFO_PREFIX = "/collective_group/";

struct CollectiveGroupInfo {
    std::vector<std::string> instances;
    int worldSize;
    std::vector<int> ranks;
    const std::string groupName;
    Backend backend;

    std::string ToString()
    {
        nlohmann::json j =
            nlohmann::json{{"instances", instances}, {"worldSize", worldSize}, {"ranks", ranks}, {"backend", backend}};
        return j.dump();
    }

    void FromJson(const std::string &str)
    {
        nlohmann::json j;
        try {
            j = nlohmann::json::parse(str);
        } catch (const std::exception &e) {
            YRLOG_WARN("json parse error: {}", e.what());
            return;
        }

        j.at("instances").get_to(instances);
        j.at("worldSize").get_to(worldSize);
        j.at("ranks").get_to(ranks);
        j.at("backend").get_to(backend);
    }
};

int CollectiveGroup::GetRank() const
{
    return rank_;
}

std::string CollectiveGroup::GetGroupName()
{
    return groupName_;
}

Backend CollectiveGroup::GetBackend()
{
    return backend_;
}

int CollectiveGroup::GetWorldSize() const
{
    return worldSize_;
}

void InitCollectiveGroup(int worldSize, int rank, const std::string &groupName, Backend backend)
{
    CollectiveGroupMgr::GetInstance().InitCollectiveGroup(worldSize, rank, groupName, backend);
}

void CreateCollectiveGroup(const std::vector<std::string> &instanceIDs, int worldSize, const std::vector<int> &ranks,
                           const std::string &groupName, Backend backend)
{
    // todo 前置参数校验
    CollectiveGroupInfo info{
        .instances = instanceIDs, .worldSize = worldSize, .ranks = ranks, .groupName = groupName, .backend = backend};

    YR::KVManager::Set(COLLECTIVE_GROUP_INFO_PREFIX + groupName, info.ToString());
}

void DestroyCollectiveGroup(const std::string &groupName)
{
    CollectiveGroupMgr::GetInstance().DestroyCollectiveGroup(groupName);
}

void AllReduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op, const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->AllReduce(input, output, op);
}

void Reduce(const DataDescriptor &input, DataDescriptor &output, const ReduceOp &op, int dstRank,
            const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->Reduce(input, output, op, dstRank);
}

void AllGather(const DataDescriptor &input, DataDescriptor &output, const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->AllGather(input, output);
}

void Barrier(const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->Barrier();
}

void Scatter(const DataDescriptor &input, DataDescriptor &output, int srcRank, const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->Scatter(input, output, srcRank);
}

void Broadcast(const DataDescriptor &input, DataDescriptor &output, int srcRank, const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->Barrier();
}

void Recv(DataDescriptor &output, int srcRank, int tag, const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->Recv(output, srcRank, tag);
}

void Send(const DataDescriptor &input, int dstRank, int tag, const std::string &groupName)
{
    auto group = CollectiveGroupMgr::GetInstance().CheckAndCreateGroup(groupName);
    THROW_IF_TRUE(group == nullptr, YR::Libruntime::ErrorCode::ERR_PARAM_INVALID,
                  "group: " + groupName + " is not init");
    group->Send(input, dstRank, tag);
}

std::shared_ptr<CollectiveGroup> CollectiveGroupMgr::CheckAndCreateGroup(const std::string &groupName)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);  // todo 看下锁的范围
    if (groups_.find(groupName) != groups_.end()) {
        return groups_[groupName];
    }
    auto str = YR::KVManager::Get(COLLECTIVE_GROUP_INFO_PREFIX + groupName);
    THROW_IF_TRUE(str.empty(), YR::Libruntime::ErrorCode::ERR_PARAM_INVALID, "group: " + groupName + " is not init");

    CollectiveGroupInfo info;
    info.FromJson(str);

    int index = 0;
    for (; index < info.instances.size(); ++index) {
        if (info.instances.at(index) == "GetInstanceID") { // todo 获取instanceID
            break;
        }
    }

    THROW_IF_TRUE(index == info.instances.size(), YR::Libruntime::ErrorCode::ERR_PARAM_INVALID, "invalid instance id");
    InitCollectiveGroup(info.worldSize, info.ranks[index], groupName, info.backend);
    return groups_[groupName];
}

void CollectiveGroupMgr::InitCollectiveGroup(int worldSize, int rank, const std::string &groupName, Backend backend)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (groups_.find(groupName) != groups_.end()) {
        YRLOG_DEBUG("collective group({}) already existed", groupName);
        throw YR::Exception(YR::Libruntime::ErrorCode::ERR_PARAM_INVALID, "collective group already existed");
    }

    std::shared_ptr<CollectiveGroup> group;
    switch (backend) {
        case Backend::GLOO:
            group = std::make_shared<GlooCollectiveGroup>(groupName, worldSize, rank);
            break;

        case Backend::INVALID:
            // fall-through
        default:
            YRLOG_ERROR("failed to init collective group({}), invalid backend: {}", groupName, backend);
            throw YR::Exception(YR::Libruntime::ErrorCode::ERR_PARAM_INVALID, "invalid collective group backend");
    }

    groups_[groupName] = group;
}

void CollectiveGroupMgr::DestroyCollectiveGroup(const std::string &groupName)
{
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    groups_.erase(groupName);
}

}  // namespace YR::collective