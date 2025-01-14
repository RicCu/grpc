/*
 *
 * Copyright 2018 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef GRPC_CORE_EXT_FILTERS_CLIENT_CHANNEL_LB_POLICY_XDS_XDS_LOAD_BALANCER_API_H
#define GRPC_CORE_EXT_FILTERS_CLIENT_CHANNEL_LB_POLICY_XDS_XDS_LOAD_BALANCER_API_H

#include <grpc/support/port_platform.h>

#include <grpc/slice_buffer.h>

#include "src/core/ext/filters/client_channel/lb_policy/xds/xds_client_stats.h"
#include "src/core/ext/filters/client_channel/server_address.h"

namespace grpc_core {

struct XdsLocalityInfo {
  bool operator==(const XdsLocalityInfo& other) const {
    return *locality_name == *other.locality_name &&
           serverlist == other.serverlist && lb_weight == other.lb_weight &&
           priority == other.priority;
  }

  // This comparator only compares the locality names.
  struct Less {
    bool operator()(const XdsLocalityInfo& lhs, const XdsLocalityInfo& rhs) {
      return XdsLocalityName::Less()(lhs.locality_name, rhs.locality_name);
    }
  };

  RefCountedPtr<XdsLocalityName> locality_name;
  ServerAddressList serverlist;
  uint32_t lb_weight;
  uint32_t priority;
};

using XdsLocalityList = InlinedVector<XdsLocalityInfo, 1>;

struct XdsUpdate {
  XdsLocalityList locality_list;
  // TODO(juanlishen): Pass drop_per_million when adding drop support.
};

// Creates an EDS request querying \a service_name.
grpc_slice XdsEdsRequestCreateAndEncode(const char* service_name);

// Parses the EDS response and returns the args to update locality map. If there
// is any error, the output update is invalid.
grpc_error* XdsEdsResponseDecodeAndParse(const grpc_slice& encoded_response,
                                         XdsUpdate* update);

// Creates an LRS request querying \a server_name.
grpc_slice XdsLrsRequestCreateAndEncode(const char* server_name);

// Creates an LRS request sending client-side load reports. If all the counters
// in \a client_stats are zero, returns empty slice.
grpc_slice XdsLrsRequestCreateAndEncode(const char* server_name,
                                        XdsClientStats* client_stats);

// Parses the LRS response and returns the client-side load reporting interval.
// If there is any error (e.g., the found server name doesn't match \a
// expected_server_name), the output config is invalid.
grpc_error* XdsLrsResponseDecodeAndParse(const grpc_slice& encoded_response,
                                         grpc_millis* load_reporting_interval,
                                         const char* expected_server_name);

}  // namespace grpc_core

#endif /* GRPC_CORE_EXT_FILTERS_CLIENT_CHANNEL_LB_POLICY_XDS_XDS_LOAD_BALANCER_API_H \
        */
