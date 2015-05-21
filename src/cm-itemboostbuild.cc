/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#include <fnord-base/stdtypes.h>
#include <fnord-base/cli/flagparser.h>
#include <fnord-base/application.h>
#include <fnord-base/logging.h>
#include <fnord-base/random.h>
#include <fnord-base/wallclock.h>
#include <fnord-base/thread/eventloop.h>
#include <fnord-base/thread/threadpool.h>
#include <fnord-http/httpconnectionpool.h>
#include <fnord-dproc/Application.h>
#include <fnord-dproc/LocalScheduler.h>
#include <fnord-tsdb/TSDBClient.h>
#include "analytics/ItemBoostMapper.h"
#include "analytics/SSTableMergeReducer.h"

using namespace fnord;
using namespace cm;

fnord::thread::EventLoop ev;

int main(int argc, const char** argv) {
  fnord::Application::init();
  fnord::Application::logToStderr();

  fnord::cli::FlagParser flags;

  flags.defineFlag(
      "tempdir",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      NULL,
      "artifact directory",
      "<path>");

  flags.defineFlag(
      "loglevel",
      fnord::cli::FlagParser::T_STRING,
      false,
      NULL,
      "INFO",
      "loglevel",
      "<level>");

  flags.parseArgv(argc, argv);

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  /* start event loop */
  auto evloop_thread = std::thread([] {
    ev.run();
  });

  http::HTTPConnectionPool http(&ev);
  tsdb::TSDBClient tsdb("http://nue03.prod.fnrd.net:7003/tsdb", &http);

  dproc::Application app("cm.itemboost");

  app.registerProtoTaskFactory<AnalyticsTableScanMapperParams>(
      "ItemBoostMapper",
      [&tsdb] (const AnalyticsTableScanMapperParams& params)
          -> RefPtr<dproc::Task> {
        auto report = new ItemBoostMapper(
            new AnalyticsTableScanSource(params, &tsdb),
            new SSTableSink<ItemBoostRowCRDT>());

        report->setCacheKey(
            "cm.itemboost~" + report->input()->cacheKey());

        return report;
      });

  app.registerProtoTaskFactory<AnalyticsTableScanReducerParams>(
      "ItemBoostReducer",
      [&tsdb] (const AnalyticsTableScanReducerParams& params)
          -> RefPtr<dproc::Task> {
        auto stream = "joined_sessions." + params.customer();
        auto partitions = tsdb.listPartitions(
            stream,
            params.from_unixmicros(),
            params.until_unixmicros());

        List<dproc::TaskDependency> map_chunks;
        for (const auto& part : partitions) {
          AnalyticsTableScanMapperParams map_chunk_params;
          map_chunk_params.set_stream_key(stream);
          map_chunk_params.set_partition_key(part);

          map_chunks.emplace_back(dproc::TaskDependency {
            .task_name = "ItemBoostMapper",
            .params = *msg::encode(map_chunk_params)
          });
        }

        return new SSTableMergeReducer<ItemBoostRowCRDT>(
            new SSTableSource<ItemBoostRowCRDT>(map_chunks),
            new SSTableSink<ItemBoostRowCRDT>());
      });


  dproc::LocalScheduler sched(flags.getString("tempdir"));
  sched.start();

  AnalyticsTableScanReducerParams params;
  params.set_customer("dawanda");
  params.set_from_unixmicros(WallClock::unixMicros() - 30 * kMicrosPerDay);
  params.set_until_unixmicros(WallClock::unixMicros() - 2 * kMicrosPerDay);

  auto res = sched.run(&app, "ItemBoostReducer", *msg::encode(params));

  auto output_file = File::openFile(
      flags.getString("output"),
      File::O_CREATEOROPEN | File::O_WRITE);

  output_file.write(res->data(), res->size());

  sched.stop();
  ev.shutdown();
  evloop_thread.join();
  exit(0);
}

