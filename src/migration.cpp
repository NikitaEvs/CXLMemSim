//
// Created by victoryang00 on 4/17/23.
//

#include "migration.h"

Migration::Migration() {} //parameter: interval, how much to migrate
int Migration::compute_once(CXLController *) {
//consumer: gain a snapshot hot entry of expander, need to check whether it's hitting the old.
//producer: remove the entry from expander

    return 0;
}
int Migration::get_bandwidth(int id) { return 0; }
//callback