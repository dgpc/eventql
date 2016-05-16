/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _FNORD_LOGTABLE_TABLEREPLICATION_H
#define _FNORD_LOGTABLE_TABLEREPLICATION_H
#include <thread>
#include <eventql/util/stdtypes.h>
#include <eventql/util/uri.h>
#include <fnord-logtable/TableRepository.h>
#include "eventql/util/http/httprequest.h"
#include "eventql/util/http/httpconnectionpool.h"

namespace util {
namespace logtable {

class TableReplication {
public:

  TableReplication(http::HTTPConnectionPool* http);

  void start();
  void stop();

  void replicateTableFrom(
      RefPtr<TableWriter> table,
      const URI& source_uri);

protected:

  void pull(
      RefPtr<TableWriter> table,
      const URI& source_uri);

  void run();

  uint64_t interval_;
  std::atomic<bool> running_;
  std::thread thread_;

  Vector<Pair<RefPtr<TableWriter>, URI>> targets_;
  http::HTTPConnectionPool* http_;
};

} // namespace logtable
} // namespace util

#endif