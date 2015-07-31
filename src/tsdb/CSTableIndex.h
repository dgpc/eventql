/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stx/stdtypes.h>
#include <tsdb/TSDBTableScanSpec.pb.h>
#include <tsdb/TSDBService.h>
#include <stx/protobuf/MessageSchema.h>
#include <stx/random.h>
#include <dproc/BlobRDD.h>

using namespace stx;

namespace tsdb {

class CSTableIndex : public dproc::BlobRDD {
public:

  CSTableIndex(
      const TSDBTableScanSpec params,
      tsdb::TSDBService* tsdb);

  RefPtr<VFSFile> computeBlob(dproc::TaskContext* context) override;

  dproc::StorageLevel storageLevel() const override {
    return dproc::StorageLevel::MEMORY;
  }

protected:
  TSDBTableScanSpec params_;
  tsdb::TSDBService* tsdb_;
};

}
