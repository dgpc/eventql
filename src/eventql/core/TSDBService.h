/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_TSDB_TSDBNODE_H
#define _FNORD_TSDB_TSDBNODE_H
#include <eventql/util/stdtypes.h>
#include <eventql/util/random.h>
#include <eventql/util/option.h>
#include <eventql/util/protobuf/DynamicMessage.h>
#include <eventql/util/thread/queue.h>
#include <eventql/util/thread/eventloop.h>
#include <eventql/util/mdb/MDB.h>
#include <eventql/core/TableConfig.pb.h>
#include <eventql/core/Partition.h>
#include <eventql/core/TSDBNodeConfig.pb.h>
#include <eventql/core/TSDBTableInfo.h>
#include <eventql/core/PartitionInfo.pb.h>
#include <eventql/core/RecordEnvelope.pb.h>
#include <eventql/core/PartitionMap.h>
#include <eventql/core/TimeWindowPartitioner.h>

using namespace stx;

namespace zbase {

enum class InsertFlags : uint64_t {
  REPLICATED_WRITE = 1,
  SYNC_COMMIT = 2
};

class TSDBService {
public:

  TSDBService(
      PartitionMap* pmap,
      ReplicationScheme* repl,
      thread::EventLoop* ev,
      http::HTTPClientStats* http_stats);

  void createTable(const TableDefinition& config);

  void listTables(
      const String& tsdb_namespace,
      Function<void (const TSDBTableInfo& table)> fn) const;

  void listTablesReverse(
      const String& tsdb_namespace,
      Function<void (const TSDBTableInfo& table)> fn) const;

  void insertRecords(
      const RecordEnvelopeList& records,
      uint64_t flags = 0);

  void insertRecords(
      const Vector<RecordEnvelope>& records,
      uint64_t flags = 0);

  void insertRecords(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key,
      const Vector<RecordRef>& records,
      uint64_t flags = 0);

  void insertRecord(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key,
      const SHA1Hash& record_id,
      uint64_t record_version,
      const Buffer& record,
      uint64_t flags = 0);

  void insertRecord(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& record_id,
      uint64_t record_version,
      const json::JSONObject::const_iterator& data_begin,
      const json::JSONObject::const_iterator& data_end,
      uint64_t flags = 0);

  void insertRecord(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& record_id,
      uint64_t record_version,
      const msg::DynamicMessage& data,
      uint64_t flags = 0);

  void compactPartition(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key);

  void updatePartitionCSTable(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key,
      const String& tmpfile_path,
      uint64_t version);

  void fetchPartition(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key,
      Function<void (const Buffer& record)> fn);

  void fetchPartitionWithSampling(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key,
      size_t sample_modulo,
      size_t sample_index,
      Function<void (const Buffer& record)> fn);

  Vector<TimeseriesPartition> listPartitions(
      const String& tsdb_namespace,
      const String& table_key,
      const UnixTime& from,
      const UnixTime& until);

  Option<PartitionInfo> partitionInfo(
      const String& tsdb_namespace,
      const String& table_key,
      const SHA1Hash& partition_key);

  Option<RefPtr<msg::MessageSchema>> tableSchema(
      const String& tsdb_namespace,
      const String& table_key);

  Option<TableDefinition> tableConfig(
      const String& tsdb_namespace,
      const String& table_key);

  Option<RefPtr<TablePartitioner>> tablePartitioner(
      const String& tsdb_namespace,
      const String& table_key);

protected:

  void insertRecordsLocal(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key,
      const Vector<RecordRef>& records,
      uint64_t flags);

  void insertRecordsRemote(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key,
      const Vector<RecordRef>& records,
      uint64_t flags,
      const ReplicaRef& host);

  PartitionMap* pmap_;
  ReplicationScheme* repl_;
  http::HTTPConnectionPool http_;
};

} // namespace tdsb

#endif