
function funcIconText(funcInfo) {
    /// 获取集成应用图标 TEXT
    if (funcInfo.iconImageFile.length>0) {
        /// 有配置图标，返回空
        return "";
    }
    return funcIconTextBySubscribeId(funcInfo.subscribeId);
}

function funcIconTextBySubscribeId(subscribeId) {
    // 1002300102 = 找群找人
    // 1002300103 = 我的消息
    // 1002300104 = 我的邮件
    // ~1002300105 = 群共享
    // 1002300106 = 个人收藏
    // 1002300110 = 工作台
    // 1002300111 = 计划
    // 1002300112 = 任务
    // 1002300113 = 日报
    // ~1002300114 = 报告
    // ~1002300115 = 考勤
    // 1002301100 = 新建计划 from_sub_id=1002300111（计划）
    // 1002301102 = 新建任务 from_sub_id=1002300112（任务）

    if (subscribeId==1002300102) {
        return "\uf002";
    }
    else if (subscribeId==1002300103) {
        return "\uf27a";
        //            return "\uf27b";
        //            return "\uf1d9";
    }
    else if (subscribeId==1002300104) {
        return "\uf0e0";
    }
    else if (subscribeId==1002300106) {
        return "\uf1b2";
    }
    else if (subscribeId==1002300110) {
        return "\uf015";
        //            return "\uf0ce";
    }
    else if (subscribeId==1002300111) {
        return "\uf03a";
    }
    else if (subscribeId==1002300112) {
        return "\uf0ae";
    }
    else if (subscribeId==1002300113) {
        return "\uf044";
    }
    //        else if (subscribeId==1002300115) {
    //            return "\uf044";
    //        }

    /// 返回默认集成应用图标
    return "\uf009";
}
