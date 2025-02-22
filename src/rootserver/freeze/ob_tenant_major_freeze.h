/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef OCEANBASE_ROOTSERVER_FREEZE_OB_TENANT_MAJOR_FREEZE_
#define OCEANBASE_ROOTSERVER_FREEZE_OB_TENANT_MAJOR_FREEZE_

#include "rootserver/freeze/ob_major_merge_scheduler.h"
#include "rootserver/freeze/ob_freeze_info_manager.h"
#include "rootserver/freeze/ob_major_merge_progress_checker.h"
#include "rootserver/freeze/ob_zone_merge_manager.h"
#include "rootserver/freeze/ob_freeze_info_detector.h"
#include "rootserver/freeze/ob_daily_major_freeze_launcher.h"

namespace oceanbase
{
namespace share
{
class ObIServerTrace;
namespace schema
{
class ObMultiVersionSchemaService;
}
}
namespace rootserver
{

// major freeze for tenant level merge
// 1. generate freeze info
// 2. schedule major merge
// 3. check major merge whether finish
// 4. do checksum check
class ObTenantMajorFreeze
{
public:
  ObTenantMajorFreeze();
  virtual ~ObTenantMajorFreeze();
  int init(const uint64_t tenant_id,
           common::ObMySQLProxy &sql_proxy,
           common::ObServerConfig &config,
           share::schema::ObMultiVersionSchemaService &schema_service,
           share::ObIServerTrace &server_trace);

  int start();
  void stop();
  int wait();
  int destroy();

  // for switch_role fastly
  void pause();
  void resume();

  uint64_t get_tenant_id() const { return tenant_id_; }

  int get_frozen_scn(share::SCN &frozen_scn);
  int get_global_broadcast_scn(share::SCN &global_broadcast_scn) const;

  int launch_major_freeze();

  int suspend_merge();

  int resume_merge();

  int clear_merge_error();

private:
  // major merge one by one
  static const int64_t UNMERGED_VERSION_LIMIT = 1;

  int check_freeze_info();
  int check_tenant_status() const;

  int set_freeze_info();

private:
  bool is_inited_;
  uint64_t tenant_id_;

  ObZoneMergeManager zone_merge_mgr_;
  ObFreezeInfoManager freeze_info_mgr_;
  ObFreezeInfoDetector freeze_info_detector_;
  ObMajorMergeScheduler merge_scheduler_;
  ObDailyMajorFreezeLauncher daily_launcher_;

  share::schema::ObMultiVersionSchemaService *schema_service_;

  DISALLOW_COPY_AND_ASSIGN(ObTenantMajorFreeze);
};

} // end namespace rootserver
} // end namespace oceanbase

#endif // OCEANBASE_FREEZE_ROOTSERVER_OB_TENANT_MAJOR_FREEZE_
