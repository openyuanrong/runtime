# Copyright (c) Huawei Technologies Co., Ltd. 2026. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

BLOG_DATA = [
    {
        "title": "Breaking the Multi-Tenant Dilemma in Distributed Computing: openYuanrong Multi-Tenant Isolation",
        "description": (
                       "As a general-purpose Serverless distributed computing engine, openYuanrong has fully "
                       "considered multi-tenant isolation since its inception. "
                       "It supports multiple tenants sharing the same cluster, while deploying FunctionProxy "
                       "and DataWorker components as DaemonSets to achieve node-level resource sharing, "
                       "effectively accelerating cold start efficiency for tenant workloads."
        ),
        "date": "2026-03-04",
        "read_time": "8 min",
        "link": "https://www.openeuler.openatom.cn/en/blog/20260316-openYuanrong_06/20260316-openYuanrong_06.html"
    },
    {
        "title": "openYuanrong DataFlow: High-Performance Distributed Data Transfer Near Compute",
        "description": (
                       "openYuanrong DataFlow is a stream semantics feature based on shared memory Pages "
                       "provided by the openYuanrong data system, designed to eliminate redundant copying "
                       "and relay overhead in traditional message middleware. The data system includes three "
                       "components: Client, Worker, and Directory."
        ),
        "date": "2026-02-28",
        "read_time": "10 min",
        "link": "https://www.openeuler.openatom.cn/en/blog/20260316-openYuanrong_05/20260316-openYuanrong_05.html"
    },
    {
        "title": "YuanRong: A Production General-purpose Serverless System for Distributed Applications in the Cloud",
        "description": (
            "We design, implement, and evaluate YuanRong, the first production general-purpose serverless platform "
            "with a unified programming interface, multi-language runtime, and a distributed computing kernel for "
            "cloud-based applications."
        ),
        "date": "2024-08-04",
        "read_time": "30 min",
        "link": "https://dl.acm.org/doi/10.1145/3651890.3672216"
    },
]