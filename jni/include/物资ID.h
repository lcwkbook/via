#include <map>
#include <string>
#include <unordered_map>

inline const char* GetItems(int Id) {
    switch (Id) {
        case 9801001:          
            return "卧底证";
            break;
        case 9801002:
            return "能源电池";
            break;
        case 9801003:
            return "加密房间房卡";
            break;
        case 9801004:
            return "勘察情报";
            break;
        case 9802001:
            return "金库密码卡-白";
            break;
        case 9802002:
            return "金库密码卡-黄";
            break;
        case 9802003:
            return "金库密码卡-红";
            break;
        case 9802004:
            return "金库密码卡-绿";
            break;
        case 9802005:
            return "金库密码卡-黑";
            break;
        case 9802006:
            return "功勋奖章（铜）";
            break;
        case 9802007:
            return "功勋奖章（银）";
            break;
        case 9802008:
            return "功勋奖章（金）";
            break;
        case 9802009:
            return "撤离信号电话机";
            break;
        case 9803001:
            return "1级变卖物测试";
            break;
        case 9803002:
            return "2级变卖物测试";
            break;
        case 9803003:
            return "3级变卖物测试";
            break;
        case 9803004:
            return "4级变卖物测试";
            break;
        case 9803005:
            return "5级变卖物测试";
            break;
        case 9803006:
            return "6级变卖物测试";
            break;
        case 9803007:
            return "7级变卖物测试";
            break;
        case 9803008:
            return "指南针";
            break;
        case 9803009:
            return "明信片";
            break;
        case 9803010:
            return "旧式录像带";
            break;
        case 9803011:
            return "扑克牌";
            break;
        case 9803012:
            return "军用水壶";
            break;
        case 9803013:
            return "罐头";
            break;
        case 9803014:
            return "杂志";
            break;
        case 9803015:
            return "便携手礼";
            break;
        case 9803016:
            return "旅行手册";
            break;
        case 9803017:
            return "钱";
            break;
        case 9803018:
            return "零件包";
            break;
        case 9803019:
            return "机油";
            break;
        case 9803020:
            return "怀表";
            break;
        case 9803021:
            return "怡神精油";
            break;
        case 9803022:
            return "爱心项链";
            break;
        case 9803023:
            return "燃气瓶";
            break;
        case 9803024:
            return "润滑油";
            break;
        case 9803025:
            return "汽车钥匙";
            break;
        case 9803026:
            return "柴油";
            break;
        case 9803027:
            return "军用手表";
            break;
        case 9803028:
            return "高级钱";
            break;
        case 9803029:
            return "科技部件";
            break;
        case 9803030:
            return "狗牌";
            break;
        case 9803031:
            return "信号发生器";
            break;
        case 9803032:
            return "净水器";
            break;
        case 9803033:
            return "CPU处理器";
            break;
        case 9803034:
            return "探测器";
            break;
        case 9803035:
            return "平板电脑";
            break;
        case 9803036:
            return "军用电池";
            break;
        case 9803037:
            return "军用电路板";
            break;
        case 9803038:
            return "精密仪器蓝图";
            break;
        case 9803039:
            return "绝密情报";
            break;
        case 9803040:
            return "设计图纸";
            break;
        case 9803041:
            return "显卡";
            break;
        case 9803042:
            return "镜头";
            break;
        case 9803043:
            return "珍稀古钱币";
            break;
        case 9803044:
            return "一根金条";
            break;
        case 9803045:
            return "光子鸡奖杯";
            break;
        case 9803046:
            return "野兽碎骨";
            break;
        case 9803047:
            return "野兽头骨";
            break;
        case 9803048:
            return "异变的肩骨";
            break;
        case 9803049:
            return "装甲碎片";
            break;
        case 9803050:
            return "装甲能源部件";
            break;
        case 9803051:
            return "装甲爆发部件";
            break;
        case 9803052:
            return "紫矿碎片";
            break;
        case 9803053:
            return "紫矿晶块";
            break;
        case 9803054:
            return "稀有宝石";
            break;
        case 9803055:
            return "黄金烤鸡";
            break;
        case 9803056:
            return "收发信器";
            break;
        case 9803057:
            return "钛合金";
            break;
        case 9803058:
            return "制动引擎";
            break;
        case 9803059:
            return "胶卷";
            break;
        case 9803060:
            return "拉刀";
            break;
        case 9803061:
            return "打火机";
            break;
        case 9803062:
            return "黑胶唱片";
            break;
        case 9803063:
            return "窃听装置";
            break;
        case 9803064:
            return "摄像头";
            break;
        case 9803065:
            return "雷管";
            break;
        case 9803066:
            return "黑鹰勋章";
            break;
        case 9803067:
            return "金狼雕像";
            break;
        case 9803068:
            return "密信";
            break;
        case 9803069:
            return "紫色试管药剂";
            break;
        case 9803070:
            return "数据芯片";
            break;
        case 9804001:
            return "1级头盔";
            break;
        case 9804002:
            return "2级头盔";
            break;
        case 9804003:
            return "3级头盔";
            break;
        case 9804004:
            return "4级头盔";
            break;
        case 9804005:
            return "4级头盔(铁爪)";
            break;
        case 9804006:
            return "4级头盔(黑鹰)";
            break;
        case 9804007:
            return "5级头盔";
            break;
        case 9804008:
            return "5级头盔(铁爪)";
            break;
        case 9804009:
            return "5级头盔(黑鹰)";
            break;
        case 9804010:
            return "6级头盔";
            break;
        case 9804011:
            return "6级头盔(铁爪)";
            break;
        case 9804012:
            return "6级头盔(黑鹰)";
            break;
        case 9804013:
            return "7级头盔";
            break;
        case 9804014:
            return "7级头盔(铁爪)";
            break;
        case 9804015:
            return "7级头盔(黑鹰)";
            break;
        case 9804016:
            return "头戴夜视仪";
            break;
        case 9804017:
            return "7级头盔";
            break;
        case 9804018:
            return "7级头盔(铁爪)";
            break;
        case 9804019:
            return "7级头盔(黑鹰)";
            break;
        case 9804020:
            return "机甲腿甲（冲刺技能）";
            break;
        case 9805001:
            return "1级防弹衣";
            break;
        case 9805002:
            return "2级防弹衣";
            break;
        case 9805003:
            return "3级防弹衣";
            break;
        case 9805004:
            return "4级防弹衣";
            break;
        case 9805005:
            return "4级防弹衣(铁爪)";
            break;
        case 9805006:
            return "4级防弹衣(黑鹰)";
            break;
        case 9805007:
            return "5级防弹衣";
            break;
        case 9805008:
            return "5级防弹衣(铁爪)";
            break;
        case 9805009:
            return "5级防弹衣(黑鹰)";
            break;
        case 9805010:
            return "6级防弹衣";
            break;
        case 9805011:
            return "6级防弹衣(铁爪)";
            break;
        case 9805012:
            return "6级防弹衣(黑鹰)";
            break;
        case 9805013:
            return "7级防弹衣";
            break;
        case 9805014:
            return "7级防弹衣(铁爪)";
            break;
        case 9805015:
            return "7级防弹衣(黑鹰)";
            break;
        case 9805016:
            return "7级防弹衣";
            break;
        case 9805017:
            return "7级防弹衣(铁爪)";
            break;
        case 9805018:
            return "7级防弹衣(黑鹰)";
            break;
        case 9805020:
            return "机甲防具";
            break;
        case 9805021:
            return "子物品-4级防弹衣";
            break;
        case 9805022:
            return "子物品-4级防弹衣(铁爪)";
            break;
        case 9805023:
            return "子物品-4级防弹衣(黑鹰)";
            break;
        case 9805024:
            return "子物品-5级防弹衣";
            break;
        case 9805025:
            return "子物品-5级防弹衣(铁爪)";
            break;
        case 9805026:
            return "子物品-5级防弹衣(黑鹰)";
            break;
        case 9805027:
            return "子物品-6级防弹衣";
            break;
        case 9805028:
            return "子物品-6级防弹衣(铁爪)";
            break;
        case 9805029:
            return "子物品-6级防弹衣(黑鹰)";
            break;
        case 9805030:
            return "子物品-7级防弹衣";
            break;
        case 9805031:
            return "子物品-7级防弹衣(铁爪)";
            break;
        case 9805032:
            return "子物品-7级防弹衣(黑鹰)";
            break;
        case 9805033:
            return "子物品-7级防弹衣";
            break;
        case 9805034:
            return "子物品-7级防弹衣(铁爪)";
            break;
        case 9805035:
            return "子物品-7级防弹衣(黑鹰)";
            break;
        case 9805098:
            return "子物品-7级防弹衣·特劳斯";
            break;
        case 9805099:
            return "7级防弹衣·特劳斯";
            break;
        case 9806001:
            return "1级背包";
            break;
        case 9806002:
            return "2级背包";
            break;
        case 9806003:
            return "3级背包";
            break;
        case 9806004:
            return "4级背包";
            break;
        case 9806005:
            return "4级背包(铁爪)";
            break;
        case 9806006:
            return "4级背包(黑鹰)";
            break;
        case 9806007:
            return "5级背包";
            break;
        case 9806008:
            return "5级背包(铁爪)";
            break;
        case 9806009:
            return "5级背包(黑鹰)";
            break;
        case 9806010:
            return "6级背包";
            break;
        case 9806011:
            return "6级背包(铁爪)";
            break;
        case 9806012:
            return "6级背包(黑鹰)";
            break;
        case 9806013:
            return "7级背包";
            break;
        case 9806014:
            return "7级背包(铁爪)";
            break;
        case 9806015:
            return "7级背包(黑鹰)";
            break;
        case 9806016:
            return "7级背包";
            break;
        case 9806017:
            return "7级背包(铁爪)";
            break;
        case 9806018:
            return "7级背包(黑鹰)";
            break;
        case 9807001:
            return "大砍刀";
            break;
        case 9807002:
            return "撬棍";
            break;
        case 9807003:
            return "镰刀";
            break;
        case 9807004:
            return "平底锅";
            break;
        case 9807005:
            return "火焰刀·维列";
            break;
        case 9808001:
            return "P92手枪(破损)";
            break;
        case 9808002:
            return "P92手枪(修复)";
            break;
        case 9808003:
            return "P92手枪(完好)";
            break;
        case 9808008:
            return "P1911手枪(破损)";
            break;
        case 9808009:
            return "P1911手枪(修复)";
            break;
        case 9808010:
            return "P1911手枪(完好)";
            break;
        case 9808015:
            return "R1895手枪(破损)";
            break;
        case 9808016:
            return "R1895手枪(修复)";
            break;
        case 9808017:
            return "R1895手枪(完好)";
            break;
        case 9808022:
            return "P18C手枪(破损)";
            break;
        case 9808023:
            return "P18C手枪(修复)";
            break;
        case 9808024:
            return "P18C手枪(完好)";
            break;
        case 9808029:
            return "R45手枪(破损)";
            break;
        case 9808030:
            return "R45手枪(修复)";
            break;
        case 9808031:
            return "R45手枪(完好)";
            break;
        case 9808036:
            return "短管(破损)";
            break;
        case 9808037:
            return "短管(修复)";
            break;
        case 9808038:
            return "短管(完好)";
            break;
        case 9808043:
            return "蝎式手枪(修复)";
            break;
        case 9808044:
            return "蝎式手枪(完好)";
            break;
        case 9808045:
            return "蝎式手枪(改进)";
            break;
        case 9808046:
            return "蝎式手枪(破损)";
            break;
        case 9808050:
            return "沙漠之鹰手枪(修复)";
            break;
        case 9808051:
            return "沙漠之鹰手枪(完好)";
            break;
        case 9808052:
            return "沙漠之鹰手枪(改进)";
            break;
        case 9808053:
            return "沙漠之鹰手枪(破损)";
            break;
        case 9808057:
            return "TMP-9手枪(修复)";
            break;
        case 9808058:
            return "TMP-9手枪(完好)";
            break;
        case 9808059:
            return "TMP-9手枪(改进)";
            break;
        case 9808060:
            return "空投信号枪";
            break;
        case 9808061:
            return "TMP-9手枪(破损)";
            break;
        case 9809001:
            return "十字弩(破损)";
            break;
        case 9809002:
            return "十字弩(修复)";
            break;
        case 9809003:
            return "十字弩(完好)";
            break;
        case 9809009:
            return "爆炸猎弓(骑警Boss)";
            break;
        case 9809010:
            return "爆炸猎弓·杰西";
            break;
        case 9810001:
            return "S686(破损)";
            break;
        case 9810002:
            return "S686(修复)";
            break;
        case 9810003:
            return "S686(完好)";
            break;
        case 9810008:
            return "S1897(破损)";
            break;
        case 9810009:
            return "S1897(修复)";
            break;
        case 9810010:
            return "S1897(完好)";
            break;
        case 9810015:
            return "S12K(精制)";
            break;
        case 9810016:
            return "S12K(修复)";
            break;
        case 9810017:
            return "S12K(完好)";
            break;
        case 9810018:
            return "S12K(改进)";
            break;
        case 9810022:
            return "DBS(修复)";
            break;
        case 9810023:
            return "DBS(完好)";
            break;
        case 9810024:
            return "DBS(改进)";
            break;
        case 9810025:
            return "DBS(精制)";
            break;
        case 9810029:
            return "SPAS-12(修复)";
            break;
        case 9810030:
            return "SPAS-12(完好)";
            break;
        case 9810031:
            return "SPAS-12(改进)";
            break;
        case 9810032:
            return "SPAS-12(精制)";
            break;
        case 9810036:
            return "AA12-G(修复)";
            break;
        case 9810037:
            return "AA12-G(完好)";
            break;
        case 9810038:
            return "AA12-G(改进)";
            break;
        case 9810041:
            return "AA12-G(精制)";
            break;
        case 9811001:
            return "UZI(破损)";
            break;
        case 9811002:
            return "UZI(修复)";
            break;
        case 9811003:
            return "UZI(完好)";
            break;
        case 9811004:
            return "UZI(改进)";
            break;
        case 9811008:
            return "UMP45(破损)";
            break;
        case 9811009:
            return "UMP45(修复)";
            break;
        case 9811010:
            return "UMP45(完好)";
            break;
        case 9811011:
            return "UMP45(改进)";
            break;
        case 9811015:
            return "Vector(卓越)";
            break;
        case 9811016:
            return "Vector(完好)";
            break;
        case 9811017:
            return "Vector(改进)";
            break;
        case 9811018:
            return "Vector(精制)";
            break;
        case 9811019:
            return "Vector(卓越)";
            break;
        case 9811020:
            return "Vector(黑鹰)";
            break;
        case 9811021:
            return "Vector(铁爪)";
            break;
        case 9811022:
            return "汤姆逊(破损)";
            break;
        case 9811023:
            return "汤姆逊(修复)";
            break;
        case 9811024:
            return "汤姆逊(完好)";
            break;
        case 9811025:
            return "汤姆逊(改进)";
            break;
        case 9811029:
            return "野牛(破损)";
            break;
        case 9811030:
            return "野牛(修复)";
            break;
        case 9811031:
            return "野牛(完好)";
            break;
        case 9811036:
            return "MP5K(卓越)";
            break;
        case 9811037:
            return "MP5K(完好)";
            break;
        case 9811038:
            return "MP5K(改进)";
            break;
        case 9811039:
            return "MP5K(精制)";
            break;
        case 9811040:
            return "MP5K(卓越)";
            break;
        case 9811041:
            return "MP5K(黑鹰)";
            break;
        case 9811042:
            return "MP5K(铁爪)";
            break;
        case 9811043:
            return "AKS-74U(卓越)";
            break;
        case 9811044:
            return "AKS-74U(完好)";
            break;
        case 9811045:
            return "AKS-74U(改进)";
            break;
        case 9811046:
            return "AKS-74U(精制)";
            break;
        case 9811047:
            return "AKS-74U(卓越)";
            break;
        case 9811048:
            return "AKS-74U(铁爪)";
            break;
        case 9811049:
            return "AKS-74U(黑鹰)";
            break;
        case 9811050:
            return "P90(卓越)";
            break;
        case 9811051:
            return "P90(完好)";
            break;
        case 9811052:
            return "P90(改进)";
            break;
        case 9811053:
            return "P90(精制)";
            break;
        case 9811054:
            return "P90(卓越)";
            break;
        case 9811055:
            return "P90(黑鹰)";
            break;
        case 9811056:
            return "P90(铁爪)";
            break;
        case 9812002:
            return "AKM(卓越)";
            break;
        case 9812003:
            return "AKM(完好)";
            break;
        case 9812004:
            return "AKM(改进)";
            break;
        case 9812005:
            return "AKM(精制)";
            break;
        case 9812006:
            return "AKM(卓越)";
            break;
        case 9812007:
            return "AKM(黑鹰)";
            break;
        case 9812008:
            return "AKM(铁爪)";
            break;
        case 9812009:
            return "M16A4(破损)";
            break;
        case 9812010:
            return "M16A4(修复)";
            break;
        case 9812011:
            return "M16A4(完好)";
            break;
        case 9812012:
            return "M16A4(改进)";
            break;
        case 9812015:
            return "SCAR-L(卓越)";
            break;
        case 9812016:
            return "SCAR-L(完好)";
            break;
        case 9812017:
            return "SCAR-L(改进)";
            break;
        case 9812018:
            return "SCAR-L(精制)";
            break;
        case 9812019:
            return "SCAR-L(卓越)";
            break;
        case 9812020:
            return "SCAR-L(黑鹰)";
            break;
        case 9812021:
            return "SCAR-L(铁爪)";
            break;
        case 9812022:
            return "M416(铁爪)";
            break;
        case 9812023:
            return "M416(卓越)";
            break;
        case 9812024:
            return "M416(完好)";
            break;
        case 9812025:
            return "M416(改进)";
            break;
        case 9812026:
            return "M416(精制)";
            break;
        case 9812027:
            return "M416(卓越)";
            break;
        case 9812028:
            return "M416(黑鹰)";
            break;
        case 9812029:
            return "GROZA(卓越)";
            break;
        case 9812030:
            return "GROZA(改进)";
            break;
        case 9812031:
            return "GROZA(精制)";
            break;
        case 9812032:
            return "GROZA(卓越)";
            break;
        case 9812033:
            return "GROZA(黑鹰)";
            break;
        case 9812034:
            return "GROZA(铁爪)";
            break;
        case 9812036:
            return "AUG(卓越)";
            break;
        case 9812037:
            return "AUG(改进)";
            break;
        case 9812038:
            return "AUG(精制)";
            break;
        case 9812039:
            return "AUG(卓越)";
            break;
        case 9812040:
            return "AUG(黑鹰)";
            break;
        case 9812041:
            return "AUG(铁爪)";
            break;
        case 9812043:
            return "QBZ(卓越)";
            break;
        case 9812044:
            return "QBZ(完好)";
            break;
        case 9812045:
            return "QBZ(改进)";
            break;
        case 9812046:
            return "QBZ(精制)";
            break;
        case 9812047:
            return "QBZ(卓越)";
            break;
        case 9812048:
            return "QBZ(黑鹰)";
            break;
        case 9812049:
            return "QBZ(铁爪)";
            break;
        case 9812050:
            return "M762(卓越)";
            break;
        case 9812051:
            return "M762(完好)";
            break;
        case 9812052:
            return "M762(改进)";
            break;
        case 9812053:
            return "M762(精制)";
            break;
        case 9812054:
            return "M762(卓越)";
            break;
        case 9812055:
            return "M762(黑鹰)";
            break;
        case 9812056:
            return "M762(铁爪)";
            break;
        case 9812057:
            return "Mk47(破损)";
            break;
        case 9812058:
            return "Mk47(修复)";
            break;
        case 9812059:
            return "Mk47(完好)";
            break;
        case 9812060:
            return "Mk47(改进)";
            break;
        case 9812064:
            return "G36C(卓越)";
            break;
        case 9812065:
            return "G36C(完好)";
            break;
        case 9812066:
            return "G36C(改进)";
            break;
        case 9812067:
            return "G36C(精制)";
            break;
        case 9812068:
            return "G36C(卓越)";
            break;
        case 9812069:
            return "G36C(黑鹰)";
            break;
        case 9812070:
            return "G36C(铁爪)";
            break;
        case 9812071:
            return "AC-VAL(破损)";
            break;
        case 9812072:
            return "AC-VAL(修复)";
            break;
        case 9812073:
            return "AC-VAL(完好)";
            break;
        case 9812074:
            return "AC-VAL(改进)";
            break;
        case 9812078:
            return "蜜獾(卓越)";
            break;
        case 9812079:
            return "蜜獾(完好)";
            break;
        case 9812080:
            return "蜜獾(改进)";
            break;
        case 9812081:
            return "蜜獾(精制)";
            break;
        case 9812082:
            return "蜜獾(卓越)";
            break;
        case 9812083:
            return "蜜獾(黑鹰)";
            break;
        case 9812084:
            return "蜜獾(铁爪)";
            break;
        case 9812085:
            return "Famas(卓越)";
            break;
        case 9812086:
            return "Famas(完好)";
            break;
        case 9812087:
            return "Famas(改进)";
            break;
        case 9812088:
            return "Famas(精制)";
            break;
        case 9812089:
            return "Famas(卓越)";
            break;
        case 9812090:
            return "Famas(黑鹰)";
            break;
        case 9812091:
            return "Famas(铁爪)";
            break;
        case 9812092:
            return "M416·卡德尔";
            break;
        case 9813001:
            return "M249(卓越)";
            break;
        case 9813002:
            return "M249(改进)";
            break;
        case 9813003:
            return "M249(精制)";
            break;
        case 9813004:
            return "M249(卓越)";
            break;
        case 9813005:
            return "M249(黑鹰)";
            break;
        case 9813006:
            return "M249(铁爪)";
            break;
        case 9813008:
            return "DP-28(破损)";
            break;
        case 9813009:
            return "DP-28(修复)";
            break;
        case 9813010:
            return "DP-28(完好)";
            break;
        case 9813022:
            return "MG3(卓越)";
            break;
        case 9813023:
            return "MG3(改进)";
            break;
        case 9813024:
            return "MG3(精制)";
            break;
        case 9813025:
            return "MG3(卓越)";
            break;
        case 9813026:
            return "MG3(黑鹰)";
            break;
        case 9813027:
            return "MG3(铁爪)";
            break;
        case 9813029:
            return "PKM(卓越)";
            break;
        case 9813030:
            return "PKM(改进)";
            break;
        case 9813031:
            return "PKM(精制)";
            break;
        case 9813032:
            return "PKM(卓越)";
            break;
        case 9813033:
            return "PKM(黑鹰)";
            break;
        case 9813034:
            return "PKM(铁爪)";
            break;
        case 9814001:
            return "Kar98K(修复)";
            break;
        case 9814002:
            return "Kar98K(完好)";
            break;
        case 9814003:
            return "Kar98K(改进)";
            break;
        case 9814008:
            return "M24(卓越)";
            break;
        case 9814009:
            return "M24(完好)";
            break;
        case 9814010:
            return "M24(改进)";
            break;
        case 9814011:
            return "M24(精制)";
            break;
        case 9814012:
            return "M24(卓越)";
            break;
        case 9814013:
            return "M24(黑鹰)";
            break;
        case 9814014:
            return "M24(铁爪)";
            break;
        case 9814015:
            return "AWM(卓越)";
            break;
        case 9814016:
            return "AWM(改进)";
            break;
        case 9814017:
            return "AWM(精制)";
            break;
        case 9814018:
            return "AWM(卓越)";
            break;
        case 9814019:
            return "AWM(黑鹰)";
            break;
        case 9814020:
            return "AWM(铁爪)";
            break;
        case 9814022:
            return "莫辛纳甘(破损)";
            break;
        case 9814023:
            return "莫辛纳甘(修复)";
            break;
        case 9814024:
            return "莫辛纳甘(完好)";
            break;
        case 9814029:
            return "Win94(破损)";
            break;
        case 9814030:
            return "Win94(修复)";
            break;
        case 9814031:
            return "Win94(完好)";
            break;
        case 9814036:
            return "AMR(卓越)";
            break;
        case 9814037:
            return "AMR(改进)";
            break;
        case 9814038:
            return "AMR(精制)";
            break;
        case 9814039:
            return "AMR(卓越)";
            break;
        case 9814040:
            return "AMR(黑鹰)";
            break;
        case 9814041:
            return "AMR(铁爪)";
            break;
        case 9814043:
            return "M200(卓越)";
            break;
        case 9814044:
            return "M200(完好)";
            break;
        case 9814045:
            return "M200(改进)";
            break;
        case 9814046:
            return "M200(精制)";
            break;
        case 9814047:
            return "M200(卓越)";
            break;
        case 9814048:
            return "M200(黑鹰)";
            break;
        case 9814049:
            return "M200(铁爪)";
            break;
        case 9815001:
            return "SKS(卓越)";
            break;
        case 9815002:
            return "SKS(完好)";
            break;
        case 9815003:
            return "SKS(改进)";
            break;
        case 9815004:
            return "SKS(精制)";
            break;
        case 9815005:
            return "SKS(卓越)";
            break;
        case 9815006:
            return "SKS(黑鹰)";
            break;
        case 9815007:
            return "SKS(铁爪)";
            break;
        case 9815008:
            return "VSS(破损)";
            break;
        case 9815009:
            return "VSS(修复)";
            break;
        case 9815010:
            return "VSS(完好)";
            break;
        case 9815015:
            return "Mini14(破损)";
            break;
        case 9815016:
            return "Mini14(修复)";
            break;
        case 9815017:
            return "Mini14(完好)";
            break;
        case 9815018:
            return "Mini14(改进)";
            break;
        case 9815022:
            return "Mk14(卓越)";
            break;
        case 9815023:
            return "Mk14(完好)";
            break;
        case 9815024:
            return "Mk14(改进)";
            break;
        case 9815025:
            return "Mk14(精制)";
            break;
        case 9815026:
            return "Mk14(卓越)";
            break;
        case 9815027:
            return "Mk14(黑鹰)";
            break;
        case 9815028:
            return "Mk14(铁爪)";
            break;
        case 9815029:
            return "SLR(卓越)";
            break;
        case 9815030:
            return "SLR(完好)";
            break;
        case 9815031:
            return "SLR(改进)";
            break;
        case 9815032:
            return "SLR(精制)";
            break;
        case 9815033:
            return "SLR(卓越)";
            break;
        case 9815034:
            return "SLR(黑鹰)";
            break;
        case 9815035:
            return "SLR(铁爪)";
            break;
        case 9815036:
            return "QBU(破损)";
            break;
        case 9815037:
            return "QBU(修复)";
            break;
        case 9815038:
            return "QBU(完好)";
            break;
        case 9815039:
            return "QBU(改进)";
            break;
        case 9815043:
            return "M417(卓越)";
            break;
        case 9815044:
            return "M417(完好)";
            break;
        case 9815045:
            return "M417(改进)";
            break;
        case 9815046:
            return "M417(精制)";
            break;
        case 9815047:
            return "M417(卓越)";
            break;
        case 9815048:
            return "M417(黑鹰)";
            break;
        case 9815049:
            return "M417(铁爪)";
            break;
        case 9815050:
            return "MK20-H(卓越)";
            break;
        case 9815051:
            return "MK20-H(完好)";
            break;
        case 9815052:
            return "MK20-H(改进)";
            break;
        case 9815053:
            return "MK20-H(精制)";
            break;
        case 9815054:
            return "MK20-H(卓越)";
            break;
        case 9815055:
            return "MK20-H(黑鹰)";
            break;
        case 9815056:
            return "MK20-H(铁爪)";
            break;
        case 9815064:
            return "MK12(卓越)";
            break;
        case 9815065:
            return "MK12(完好)";
            break;
        case 9815066:
            return "MK12(改进)";
            break;
        case 9815067:
            return "MK12(精制)";
            break;
        case 9815068:
            return "MK12(卓越)";
            break;
        case 9815069:
            return "MK12(黑鹰)";
            break;
        case 9815070:
            return "MK12(铁爪)";
            break;
        case 9815071:
            return "M134_装甲重机枪";
            break;
        case 9816001:
            return "钢制插板(破损)";
            break;
        case 9816002:
            return "钢制插板(修复)";
            break;
        case 9816003:
            return "钢制插板(完好)";
            break;
        case 9816004:
            return "钢制插板(改进)";
            break;
        case 9816005:
            return "钢制插板(精制)";
            break;
        case 9816006:
            return "复合纤维插板(破损)";
            break;
        case 9816007:
            return "复合纤维插板(修复)";
            break;
        case 9816008:
            return "复合纤维插板(完好)";
            break;
        case 9816009:
            return "复合纤维插板(改进)";
            break;
        case 9816010:
            return "复合纤维插板(精制)";
            break;
        case 9816011:
            return "内衬外骨骼(破损)";
            break;
        case 9816012:
            return "内衬外骨骼(修复)";
            break;
        case 9816013:
            return "内衬外骨骼(完好)";
            break;
        case 9816014:
            return "内衬外骨骼(改进)";
            break;
        case 9816015:
            return "内衬外骨骼(精制)";
            break;
        case 9816016:
            return "尼龙绑带(破损)";
            break;
        case 9816017:
            return "尼龙绑带(修复)";
            break;
        case 9816018:
            return "尼龙绑带(完好)";
            break;
        case 9816019:
            return "尼龙绑带(改进)";
            break;
        case 9816020:
            return "尼龙绑带(精制)";
            break;
        case 9816021:
            return "战术小包(破损)";
            break;
        case 9816022:
            return "战术小包(修复)";
            break;
        case 9816023:
            return "战术小包(完好)";
            break;
        case 9816024:
            return "战术小包(改进)";
            break;
        case 9816025:
            return "战术小包(精制)";
            break;
        case 9816026:
            return "信号增强器";
            break;
        case 9817001:
            return "红点瞄准镜";
            break;
        case 9817002:
            return "全息瞄准镜";
            break;
        case 9817003:
            return "2倍 瞄准镜";
            break;
        case 9817004:
            return "4倍 瞄准镜";
            break;
        case 9817005:
            return "8倍 瞄准镜";
            break;
        case 9817006:
            return "3倍 瞄准镜";
            break;
        case 9817007:
            return "6倍 瞄准镜";
            break;
        case 9817008:
            return "侧面瞄准镜";
            break;
        case 9817010:
            return "夜视瞄准镜";
            break;
        case 9817011:
            return "热成像瞄准镜";
            break;
        case 9818001:
            return "收束器(破损)";
            break;
        case 9818002:
            return "收束器(修复)";
            break;
        case 9818003:
            return "收束器(完好)";
            break;
        case 9818004:
            return "收束器(改进)";
            break;
        case 9818008:
            return "枪口补偿器()(修复)";
            break;
        case 9818009:
            return "枪口补偿器()(完好)";
            break;
        case 9818010:
            return "枪口补偿器()(改进)";
            break;
        case 9818011:
            return "枪口补偿器()(精制)";
            break;
        case 9818015:
            return "枪口补偿器()(修复)";
            break;
        case 9818016:
            return "枪口补偿器()(完好)";
            break;
        case 9818017:
            return "枪口补偿器()(改进)";
            break;
        case 9818018:
            return "枪口补偿器()(精制)";
            break;
        case 9818022:
            return "消焰器()(破损)";
            break;
        case 9818023:
            return "消焰器()(修复)";
            break;
        case 9818024:
            return "消焰器()(完好)";
            break;
        case 9818025:
            return "消焰器()(改进)";
            break;
        case 9818029:
            return "消焰器()(修复)";
            break;
        case 9818030:
            return "消焰器()(完好)";
            break;
        case 9818031:
            return "消焰器()(改进)";
            break;
        case 9818032:
            return "消焰器()(精制)";
            break;
        case 9818036:
            return "消音器(,手枪)(修复)";
            break;
        case 9818037:
            return "消音器(,手枪)(完好)";
            break;
        case 9818038:
            return "消音器(,手枪)(改进)";
            break;
        case 9818039:
            return "消音器(,手枪)(精制)";
            break;
        case 9818043:
            return "消音器()(修复)";
            break;
        case 9818044:
            return "消音器()(完好)";
            break;
        case 9818045:
            return "消音器()(改进)";
            break;
        case 9818046:
            return "消音器()(精制)";
            break;
        case 9818058:
            return "枪口补偿器(步枪)(修复)";
            break;
        case 9818059:
            return "枪口补偿器(步枪)(完好)";
            break;
        case 9818060:
            return "枪口补偿器(步枪)(改进)";
            break;
        case 9818061:
            return "枪口补偿器(步枪)(精制)";
            break;
        case 9818064:
            return "消焰器(步枪)(破损)";
            break;
        case 9818065:
            return "消焰器(步枪)(修复)";
            break;
        case 9818066:
            return "消焰器(步枪)(完好)";
            break;
        case 9818067:
            return "消焰器(步枪)(改进)";
            break;
        case 9818072:
            return "消音器(步枪)(修复)";
            break;
        case 9818073:
            return "消音器(步枪)(完好)";
            break;
        case 9818074:
            return "消音器(步枪)(改进)";
            break;
        case 9818075:
            return "消音器(步枪)(精制)";
            break;
        case 9818078:
            return "鸭嘴枪口()(破损)";
            break;
        case 9818079:
            return "鸭嘴枪口()(修复)";
            break;
        case 9818080:
            return "鸭嘴枪口()(完好)";
            break;
        case 9818081:
            return "鸭嘴枪口()(改进)";
            break;
        case 9818082:
            return "鸭嘴枪口()(精制)";
            break;
        case 9818085:
            return "延长枪管()(破损)";
            break;
        case 9818086:
            return "延长枪管()(修复)";
            break;
        case 9818087:
            return "延长枪管()(完好)";
            break;
        case 9818088:
            return "延长枪管()(改进)";
            break;
        case 9818092:
            return "延长枪管(步枪,)(破损)";
            break;
        case 9818093:
            return "延长枪管(步枪,)(修复)";
            break;
        case 9818094:
            return "延长枪管(步枪,)(完好)";
            break;
        case 9818095:
            return "延长枪管(步枪,)(改进)";
            break;
        case 9819001:
            return "扩容弹匣(,手枪)(破损)";
            break;
        case 9819002:
            return "扩容弹匣(,手枪)(修复)";
            break;
        case 9819003:
            return "扩容弹匣(,手枪)(完好)";
            break;
        case 9819004:
            return "扩容弹匣(,手枪)(改进)";
            break;
        case 9819005:
            return "扩容弹匣(,手枪)(精制)";
            break;
        case 9819008:
            return "快速弹匣(,手枪)(破损)";
            break;
        case 9819009:
            return "快速弹匣(,手枪)(修复)";
            break;
        case 9819010:
            return "快速弹匣(,手枪)(完好)";
            break;
        case 9819011:
            return "快速弹匣(,手枪)(改进)";
            break;
        case 9819012:
            return "快速弹匣(,手枪)(精制)";
            break;
        case 9819036:
            return "快速扩容弹匣(,手枪)(修复)";
            break;
        case 9819037:
            return "快速扩容弹匣(,手枪)(完好)";
            break;
        case 9819038:
            return "快速扩容弹匣(,手枪)(改进)";
            break;
        case 9819039:
            return "快速扩容弹匣(,手枪)(精制)";
            break;
        case 9819043:
            return "扩容弹匣()(破损)";
            break;
        case 9819044:
            return "扩容弹匣()(修复)";
            break;
        case 9819045:
            return "扩容弹匣()(完好)";
            break;
        case 9819046:
            return "扩容弹匣()(改进)";
            break;
        case 9819047:
            return "扩容弹匣()(精制)";
            break;
        case 9819050:
            return "快速弹匣()(破损)";
            break;
        case 9819051:
            return "快速弹匣()(修复)";
            break;
        case 9819052:
            return "快速弹匣()(完好)";
            break;
        case 9819053:
            return "快速弹匣()(改进)";
            break;
        case 9819054:
            return "快速弹匣()(精制)";
            break;
        case 9819057:
            return "快速扩容弹匣()(修复)";
            break;
        case 9819058:
            return "快速扩容弹匣()(完好)";
            break;
        case 9819059:
            return "快速扩容弹匣()(改进)";
            break;
        case 9819060:
            return "快速扩容弹匣()(精制)";
            break;
        case 9819071:
            return "扩容弹匣(步枪,机枪)(破损)";
            break;
        case 9819072:
            return "扩容弹匣(步枪,机枪)(修复)";
            break;
        case 9819073:
            return "扩容弹匣(步枪,机枪)(完好)";
            break;
        case 9819074:
            return "扩容弹匣(步枪,机枪)(改进)";
            break;
        case 9819075:
            return "扩容弹匣(步枪,机枪)(精制)";
            break;
        case 9819078:
            return "快速弹匣(步枪,机枪)(破损)";
            break;
        case 9819079:
            return "快速弹匣(步枪,机枪)(修复)";
            break;
        case 9819080:
            return "快速弹匣(步枪,机枪)(完好)";
            break;
        case 9819081:
            return "快速弹匣(步枪,机枪)(改进)";
            break;
        case 9819082:
            return "快速弹匣(步枪,机枪)(精制)";
            break;
        case 9819086:
            return "快速扩容弹匣(步枪,机枪)(修复)";
            break;
        case 9819087:
            return "快速扩容弹匣(步枪,机枪)(完好)";
            break;
        case 9819088:
            return "快速扩容弹匣(步枪,机枪)(改进)";
            break;
        case 9819089:
            return "快速扩容弹匣(步枪,机枪)(精制)";
            break;
        case 9819092:
            return "子弹袋(,)(修复)";
            break;
        case 9819093:
            return "子弹袋(,)(完好)";
            break;
        case 9819094:
            return "子弹袋(,)(改进)";
            break;
        case 9819095:
            return "子弹袋(,)(精制)";
            break;
        case 9819106:
            return "霰弹快速装填器";
            break;
        case 9819156:
            return "M134默认弹匣";
            break;
        case 9820002:
            return "战术枪托(修复)";
            break;
        case 9820003:
            return "战术枪托(完好)";
            break;
        case 9820004:
            return "战术枪托(改进)";
            break;
        case 9820005:
            return "战术枪托(精制)";
            break;
        case 9820008:
            return "托腮板()(修复)";
            break;
        case 9820009:
            return "托腮板()(完好)";
            break;
        case 9820010:
            return "托腮板()(改进)";
            break;
        case 9820011:
            return "托腮板()(精制)";
            break;
        case 9820015:
            return "箭袋(十字弩)(破损)";
            break;
        case 9820016:
            return "箭袋(十字弩)(修复)";
            break;
        case 9820017:
            return "箭袋(十字弩)(完好)";
            break;
        case 9820018:
            return "箭袋(十字弩)(改进)";
            break;
        case 9820019:
            return "箭袋(十字弩)(精制)";
            break;
        case 9820022:
            return "撞火枪托";
            break;
        case 9820023:
            return "枪托(Micro UZI)(修复)";
            break;
        case 9820024:
            return "枪托(Micro UZI)(完好)";
            break;
        case 9820025:
            return "枪托(Micro UZI)(改进)";
            break;
        case 9820026:
            return "枪托(Micro UZI)(精制)";
            break;
        case 9821001:
            return "直角前握把(破损)";
            break;
        case 9821002:
            return "直角前握把(修复)";
            break;
        case 9821003:
            return "直角前握把(完好)";
            break;
        case 9821004:
            return "直角前握把(改进)";
            break;
        case 9821005:
            return "直角前握把(精制)";
            break;
        case 9821008:
            return "垂直握把(破损)";
            break;
        case 9821009:
            return "垂直握把(修复)";
            break;
        case 9821010:
            return "垂直握把(完好)";
            break;
        case 9821011:
            return "垂直握把(改进)";
            break;
        case 9821012:
            return "垂直握把(精制)";
            break;
        case 9821015:
            return "轻型握把(破损)";
            break;
        case 9821016:
            return "轻型握把(修复)";
            break;
        case 9821017:
            return "轻型握把(完好)";
            break;
        case 9821018:
            return "轻型握把(改进)";
            break;
        case 9821019:
            return "轻型握把(精制)";
            break;
        case 9821022:
            return "半截式握把(破损)";
            break;
        case 9821023:
            return "半截式握把(修复)";
            break;
        case 9821024:
            return "半截式握把(完好)";
            break;
        case 9821025:
            return "半截式握把(改进)";
            break;
        case 9821026:
            return "半截式握把(精制)";
            break;
        case 9821029:
            return "拇指握把(破损)";
            break;
        case 9821030:
            return "拇指握把(修复)";
            break;
        case 9821031:
            return "拇指握把(完好)";
            break;
        case 9821032:
            return "拇指握把(改进)";
            break;
        case 9821033:
            return "拇指握把(精制)";
            break;
        case 9821036:
            return "激光瞄准器(破损)";
            break;
        case 9821037:
            return "激光瞄准器(修复)";
            break;
        case 9821038:
            return "激光瞄准器(完好)";
            break;
        case 9821039:
            return "激光瞄准器(改进)";
            break;
        case 9821040:
            return "激光瞄准器(精制)";
            break;
        case 9821044:
            return "下挂榴弹发射器";
            break;
        case 9821045:
            return "下挂喷火器";
            break;
        case 9823001:
            return "9毫米子弹(锈蚀)";
            break;
        case 9823002:
            return "9毫米子弹(普通)";
            break;
        case 9823003:
            return "9毫米子弹(抛光)";
            break;
        case 9823004:
            return "9毫米子弹(高爆)";
            break;
        case 9823005:
            return "9毫米子弹(爆燃)";
            break;
        case 9823006:
            return "9毫米子弹(剧毒)";
            break;
        case 9823007:
            return "5.7毫米子弹(锈蚀)";
            break;
        case 9823008:
            return "5.7毫米子弹(普通)";
            break;
        case 9823009:
            return "5.7毫米子弹(抛光)";
            break;
        case 9823010:
            return "5.7毫米子弹(高爆)";
            break;
        case 9823011:
            return "5.7毫米子弹(爆燃)";
            break;
        case 9823012:
            return "5.7毫米子弹(剧毒)";
            break;
        case 9823013:
            return "7.62毫米子弹(锈蚀)";
            break;
        case 9823014:
            return "7.62毫米子弹(普通)";
            break;
        case 9823015:
            return "7.62毫米子弹(抛光)";
            break;
        case 9823016:
            return "7.62毫米子弹(高爆)";
            break;
        case 9823017:
            return "7.62毫米子弹(爆燃)";
            break;
        case 9823018:
            return "7.62毫米子弹(剧毒)";
            break;
        case 9823019:
            return "5.56毫米子弹(锈蚀)";
            break;
        case 9823020:
            return "5.56毫米子弹(普通)";
            break;
        case 9823021:
            return "5.56毫米子弹(抛光)";
            break;
        case 9823022:
            return "5.56毫米子弹(高爆)";
            break;
        case 9823023:
            return "5.56毫米子弹(爆燃)";
            break;
        case 9823024:
            return "5.56毫米子弹(剧毒)";
            break;
        case 9823025:
            return "12口径霰弹(锈蚀)";
            break;
        case 9823026:
            return "12口径霰弹(普通)";
            break;
        case 9823027:
            return "12口径霰弹(抛光)";
            break;
        case 9823028:
            return "12口径霰弹(高爆)";
            break;
        case 9823029:
            return "12口径霰弹(爆燃)";
            break;
        case 9823030:
            return "12口径霰弹(剧毒)";
            break;
        case 9823031:
            return ".45口径子弹(锈蚀)";
            break;
        case 9823032:
            return ".45口径子弹(普通)";
            break;
        case 9823033:
            return ".45口径子弹(抛光)";
            break;
        case 9823034:
            return ".45口径子弹(高爆)";
            break;
        case 9823035:
            return ".45口径子弹(爆燃)";
            break;
        case 9823036:
            return ".45口径子弹(剧毒)";
            break;
        case 9823037:
            return ".300马格南子弹(锈蚀)";
            break;
        case 9823038:
            return ".300马格南子弹(普通)";
            break;
        case 9823039:
            return ".300马格南子弹(抛光)";
            break;
        case 9823040:
            return ".300马格南子弹(高爆)";
            break;
        case 9823041:
            return ".300马格南子弹(爆燃)";
            break;
        case 9823042:
            return ".300马格南子弹(剧毒)";
            break;
        case 9823043:
            return ".50口径子弹(锈蚀)";
            break;
        case 9823044:
            return ".50口径子弹(普通)";
            break;
        case 9823045:
            return ".50口径子弹(抛光)";
            break;
        case 9823046:
            return ".50口径子弹(高爆)";
            break;
        case 9823047:
            return ".50口径子弹(爆燃)";
            break;
        case 9823048:
            return ".50口径子弹(剧毒)";
            break;
        case 9823049:
            return ".408口径子弹(锈蚀)";
            break;
        case 9823050:
            return ".408口径子弹(普通)";
            break;
        case 9823051:
            return ".408口径子弹(抛光)";
            break;
        case 9823052:
            return ".408口径子弹(高爆)";
            break;
        case 9823053:
            return ".408口径子弹(爆燃)";
            break;
        case 9823054:
            return ".408口径子弹(剧毒)";
            break;
        case 9823060:
            return "装甲加特林子弹";
            break;
        case 9823061:
            return "40mm榴弹";
            break;
        case 9823062:
            return "火箭弹";
            break;
        case 9823063:
            return "燃气瓶";
            break;
        case 9823064:
            return "9毫米子弹(竞赛)";
            break;
        case 9823065:
            return "5.7毫米子弹(竞赛)";
            break;
        case 9823066:
            return "7.62毫米子弹(竞赛)";
            break;
        case 9823067:
            return "5.56毫米子弹(竞赛)";
            break;
        case 9823068:
            return "12口径霰弹(竞赛)";
            break;
        case 9823069:
            return ".45口径子弹(竞赛)";
            break;
        case 9823070:
            return ".300马格南子弹(竞赛)";
            break;
        case 9823071:
            return ".50口径子弹(竞赛)";
            break;
        case 9823072:
            return ".408口径子弹(竞赛)";
            break;
        case 9823073:
            return "箭矢(锈蚀)";
            break;
        case 9823074:
            return "箭矢(普通)";
            break;
        case 9823075:
            return "箭矢(抛光)";
            break;
        case 9823076:
            return "箭矢(高爆)";
            break;
        case 9823077:
            return "箭矢(爆燃)";
            break;
        case 9823078:
            return "箭矢(剧毒)";
            break;
        case 9823079:
            return "箭矢(竞赛)";
            break;
        case 9823080:
            return "E玩法信号弹";
            break;
        case 9824001:
            return "能量饮料";
            break;
        case 9824002:
            return "肾上腺素";
            break;
        case 9824003:
            return "止痛药";
            break;
        case 9824004:
            return "绷带";
            break;
        case 9824005:
            return "急救包";
            break;
        case 9824006:
            return "全能医疗箱";
            break;
        case 9824007:
            return "抗干扰药";
            break;
        case 9825001:
            return "震爆弹";
            break;
        case 9825002:
            return "烟雾弹";
            break;
        case 9825003:
            return "燃烧瓶";
            break;
        case 9825004:
            return "破片手榴弹";
            break;
        case 9826001:
            return "液氮地雷";
            break;
        case 9826002:
            return "雷鸣地雷";
            break;
        case 9826003:
            return "自救型除颤器";
            break;
        case 9826004:
            return "便携式生命探测仪";
            break;
        case 9826005:
            return "弹道追踪仪";
            break;
        case 9828001:
            return "RPG-7火箭筒";
            break;
        case 9828002:
            return "M79榴弹发射器";
            break;
        case 9828003:
            return "突击盾牌";
            break;
        default:
            return "NULL";
        break;
    }
}
inline int heldconversion(int BoxId)
{
    static const std::unordered_map<int, int> conversionMap = {
        {9808001, 106001}, {9808002, 106001}, {9808003, 106001},
        {9808008, 106002}, {9808009, 106002}, {9808010, 106002},
        {9808015, 106003}, {9808016, 106003}, {9808017, 106003},
        {9808022, 106004}, {9808023, 106004}, {9808024, 106004},
        {9808029, 106005}, {9808030, 106005}, {9808031, 106005},
        {9808036, 106006}, {9808037, 106006}, {9808038, 106006},
        {9808043, 106008}, {9808044, 106008}, {9808045, 106008}, {9808046, 106008},
        {9808050, 106010}, {9808051, 106010}, {9808052, 106010}, {9808053, 106010},
        {9808057, 106011}, {9808058, 106011}, {9808059, 106011}, {9808061, 106011},
        {9809001, 107001}, {9809002, 107001}, {9809003, 107001},
        {9809009, 107007}, {9809010, 107007},
        {9810001, 104001}, {9810002, 104001}, {9810003, 104001},
        {9810008, 104002}, {9810009, 104002}, {9810010, 104002},
        {9810015, 104003}, {9810016, 104003}, {9810017, 104003}, {9810018, 104003},
        {9810022, 104004}, {9810023, 104004}, {9810024, 104004}, {9810025, 104004},
        {9810029, 104100}, {9810030, 104100}, {9810031, 104100}, {9810032, 104100},
        {9810036, 104005}, {9810037, 104005}, {9810038, 104005}, {9810041, 104005},
        {9811001, 102001}, {9811002, 102001}, {9811003, 102001}, {9811004, 102001},
        {9811008, 102002}, {9811009, 102002}, {9811010, 102002}, {9811011, 102002},
        {9811015, 102003}, {9811016, 102003}, {9811017, 102003}, {9811018, 102003},
        {9811019, 102003}, {9811020, 102003}, {9811021, 102003},
        {9811022, 102004}, {9811023, 102004}, {9811024, 102004}, {9811025, 102004},
        {9811029, 102005}, {9811030, 102005}, {9811031, 102005},
        {9811036, 102007}, {9811037, 102007}, {9811038, 102007}, {9811039, 102007},
        {9811040, 102007}, {9811041, 102007}, {9811042, 102007},
        {9811043, 102008}, {9811044, 102008}, {9811045, 102008}, {9811046, 102008},
        {9811047, 102008}, {9811048, 102008}, {9811049, 102008},
        {9811050, 102105}, {9811051, 102105}, {9811052, 102105}, {9811053, 102105},
        {9811054, 102105}, {9811055, 102105}, {9811056, 102105},
        {9812002, 101001}, {9812003, 101001}, {9812004, 101001}, {9812005, 101001},
        {9812006, 101001}, {9812007, 101001}, {9812008, 101001},
        {9812009, 101002}, {9812010, 101002}, {9812011, 101002}, {9812012, 101002},
        {9812015, 101003}, {9812016, 101003}, {9812017, 101003}, {9812018, 101003},
        {9812019, 101003}, {9812020, 101003}, {9812021, 101003},
        {9812022, 101004}, {9812023, 101004}, {9812024, 101004}, {9812025, 101004},
        {9812026, 101004}, {9812027, 101004}, {9812028, 101004}, {9812092, 101004},
        {9812029, 101005}, {9812030, 101005}, {9812031, 101005}, {9812032, 101005},
        {9812033, 101005}, {9812034, 101005},
        {9812036, 101006}, {9812037, 101006}, {9812038, 101006}, {9812039, 101006},
        {9812040, 101006}, {9812041, 101006},
        {9812043, 101007}, {9812044, 101007}, {9812045, 101007}, {9812046, 101007},
        {9812047, 101007}, {9812048, 101007}, {9812049, 101007},
        {9812050, 101008}, {9812051, 101008}, {9812052, 101008}, {9812053, 101008},
        {9812054, 101008}, {9812055, 101008}, {9812056, 101008},
        {9812057, 101009}, {9812058, 101009}, {9812059, 101009}, {9812060, 101009},
        {9812064, 101010}, {9812065, 101010}, {9812066, 101010}, {9812067, 101010},
        {9812068, 101010}, {9812069, 101010}, {9812070, 101010},
        {9812071, 101011}, {9812072, 101011}, {9812073, 101011}, {9812074, 101011},
        {9812078, 101012}, {9812079, 101012}, {9812080, 101012}, {9812081, 101012},
        {9812082, 101012}, {9812083, 101012}, {9812084, 101012},
        {9812085, 101013}, {9812086, 101013}, {9812087, 101013}, {9812088, 101013},
        {9812089, 101013}, {9812090, 101013}, {9812091, 101013},
        {9813001, 105001}, {9813002, 105001}, {9813003, 105001}, {9813004, 105001},
        {9813005, 105001}, {9813006, 105001},
        {9813008, 105002}, {9813009, 105002}, {9813010, 105002},
        {9813022, 105010}, {9813023, 105010}, {9813024, 105010}, {9813025, 105010},
        {9813026, 105010}, {9813027, 105010},
        {9813029, 105012}, {9813030, 105012}, {9813031, 105012}, {9813032, 105012},
        {9813033, 105012}, {9813034, 105012},
        {9814001, 103001}, {9814002, 103001}, {9814003, 103001},
        {9814008, 103002}, {9814009, 103002}, {9814010, 103002}, {9814011, 103002},
        {9814012, 103002}, {9814013, 103002}, {9814014, 103002},
        {9814015, 103003}, {9814016, 103003}, {9814017, 103003}, {9814018, 103003},
        {9814019, 103003}, {9814020, 103003},
        {9814022, 103011}, {9814023, 103011}, {9814024, 103011},
        {9814029, 103008}, {9814030, 103008}, {9814031, 103008},
        {9814036, 103012}, {9814037, 103012}, {9814038, 103012}, {9814039, 103012},
        {9814040, 103012}, {9814041, 103012},
        {9814043, 103015}, {9814044, 103015}, {9814045, 103015}, {9814046, 103015},
        {9814047, 103015}, {9814048, 103015}, {9814049, 103015},
        {9814050, 103016}, {9814051, 103016}, {9814052, 103016}, {9814053, 103016},
        {9815001, 103004}, {9815002, 103004}, {9815003, 103004}, {9815004, 103004},
        {9815005, 103004}, {9815006, 103004}, {9815007, 103004},
        {9815008, 103005}, {9815009, 103005}, {9815010, 103005},
        {9815015, 103006}, {9815016, 103006}, {9815017, 103006}, {9815018, 103006},
        {9815022, 103007}, {9815023, 103007}, {9815024, 103007}, {9815025, 103007},
        {9815026, 103007}, {9815027, 103007}, {9815028, 103007},
        {9815029, 103009}, {9815030, 103009}, {9815031, 103009}, {9815032, 103009},
        {9815033, 103009}, {9815034, 103009}, {9815035, 103009},
        {9815036, 103010}, {9815037, 103010}, {9815038, 103010}, {9815039, 103010},
        {9815043, 103013}, {9815044, 103013}, {9815045, 103013}, {9815046, 103013},
        {9815047, 103013}, {9815048, 103013}, {9815049, 103013},
        {9815050, 103014}, {9815051, 103014}, {9815052, 103014}, {9815053, 103014},
        {9815054, 103014}, {9815055, 103014}, {9815056, 103014},
        {9815064, 103100}, {9815065, 103100}, {9815066, 103100}, {9815067, 103100},
        {9815068, 103100}, {9815069, 103100}, {9815070, 103100}
    };

    auto it = conversionMap.find(BoxId);
    return it != conversionMap.end() ? it->second : BoxId;
}

inline char *GetHolGunItem(int BoxId)
{
    static const std::unordered_map<int, const char*> gunMap = {
        {9808001, "P92手枪(破损)"},
        {9808002, "P92手枪(修复)"},
        {9808003, "P92手枪(完好)"},
        {9808008, "P1911手枪(破损)"},
        {9808009, "P1911手枪(修复)"},
        {9808010, "P1911手枪(完好)"},
        {9808015, "R1895手枪(破损)"},
        {9808016, "R1895手枪(修复)"},
        {9808017, "R1895手枪(完好)"},
        {9808022, "P18C手枪(破损)"},
        {9808023, "P18C手枪(修复)"},
        {9808024, "P18C手枪(完好)"},
        {9808029, "R45手枪(破损)"},
        {9808030, "R45手枪(修复)"},
        {9808031, "R45手枪(完好)"},
        {9808036, "短管(破损)"},
        {9808037, "短管(修复)"},
        {9808038, "短管(完好)"},
        {9808043, "蝎式手枪(修复)"},
        {9808044, "蝎式手枪(完好)"},
        {9808045, "蝎式手枪(改进)"},
        {9808046, "蝎式手枪(破损)"},
        {9808050, "沙漠之鹰手枪(修复)"},
        {9808051, "沙漠之鹰手枪(完好)"},
        {9808052, "沙漠之鹰手枪(改进)"},
        {9808053, "沙漠之鹰手枪(破损)"},
        {9808057, "TMP-9手枪(修复)"},
        {9808058, "TMP-9手枪(完好)"},
        {9808059, "TMP-9手枪(改进)"},
        {9808060, "空投信号枪"},
        {9808061, "TMP-9手枪(破损)"},
        {9809001, "十字弩(破损)"},
        {9809002, "十字弩(修复)"},
        {9809003, "十字弩(完好)"},
        {9809009, "爆炸猎弓(骑警Boss)"},
        {9809010, "爆炸猎弓·杰西"},
        {9810001, "S686(破损)"},
        {9810002, "S686(修复)"},
        {9810003, "S686(完好)"},
        {9810008, "S1897(破损)"},
        {9810009, "S1897(修复)"},
        {9810010, "S1897(完好)"},
        {9810015, "S12K(精制)"},
        {9810016, "S12K(修复)"},
        {9810017, "S12K(完好)"},
        {9810018, "S12K(改进)"},
        {9810022, "DBS(修复)"},
        {9810023, "DBS(完好)"},
        {9810024, "DBS(改进)"},
        {9810025, "DBS(精制)"},
        {9810029, "SPAS-12(修复)"},
        {9810030, "SPAS-12(完好)"},
        {9810031, "SPAS-12(改进)"},
        {9810032, "SPAS-12(精制)"},
        {9810036, "AA12-G(修复)"},
        {9810037, "AA12-G(完好)"},
        {9810038, "AA12-G(改进)"},
        {9810041, "AA12-G(精制)"},
        {9811001, "UZI(破损)"},
        {9811002, "UZI(修复)"},
        {9811003, "UZI(完好)"},
        {9811004, "UZI(改进)"},
        {9811008, "UMP45(破损)"},
        {9811009, "UMP45(修复)"},
        {9811010, "UMP45(完好)"},
        {9811011, "UMP45(改进)"},
        {9811015, "Vector(卓越)"},
        {9811016, "Vector(完好)"},
        {9811017, "Vector(改进)"},
        {9811018, "Vector(精制)"},
        {9811019, "Vector(卓越)"},
        {9811020, "Vector(黑鹰)"},
        {9811021, "Vector(铁爪)"},
        {9811022, "汤姆逊(破损)"},
        {9811023, "汤姆逊(修复)"},
        {9811024, "汤姆逊(完好)"},
        {9811025, "汤姆逊(改进)"},
        {9811029, "野牛(破损)"},
        {9811030, "野牛(修复)"},
        {9811031, "野牛(完好)"},
        {9811036, "MP5K(卓越)"},
        {9811037, "MP5K(完好)"},
        {9811038, "MP5K(改进)"},
        {9811039, "MP5K(精制)"},
        {9811040, "MP5K(卓越)"},
        {9811041, "MP5K(黑鹰)"},
        {9811042, "MP5K(铁爪)"},
        {9811043, "AKS-74U(卓越)"},
        {9811044, "AKS-74U(完好)"},
        {9811045, "AKS-74U(改进)"},
        {9811046, "AKS-74U(精制)"},
        {9811047, "AKS-74U(卓越)"},
        {9811048, "AKS-74U(铁爪)"},
        {9811049, "AKS-74U(黑鹰)"},
        {9811050, "P90(卓越)"},
        {9811051, "P90(完好)"},
        {9811052, "P90(改进)"},
        {9811053, "P90(精制)"},
        {9811054, "P90(卓越)"},
        {9811055, "P90(黑鹰)"},
        {9811056, "P90(铁爪)"},
        {9812002, "AKM(卓越)"},
        {9812003, "AKM(完好)"},
        {9812004, "AKM(改进)"},
        {9812005, "AKM(精制)"},
        {9812006, "AKM(卓越)"},
        {9812007, "AKM(黑鹰)"},
        {9812008, "AKM(铁爪)"},
        {9812009, "M16A4(破损)"},
        {9812010, "M16A4(修复)"},
        {9812011, "M16A4(完好)"},
        {9812012, "M16A4(改进)"},
        {9812015, "SCAR-L(卓越)"},
        {9812016, "SCAR-L(完好)"},
        {9814044, "M200(完好)"},
        {9814045, "M200(改进)"},
        {9814046, "M200(精制)"},
        {9814047, "M200(卓越)"},
        {9814048, "M200(黑鹰)"},
        {9814049, "M200(铁爪)"},
        {9814050, "SVD(改进)"},
        {9814051, "SVD(精制)"},
        {9814052, "SVD(卓越)"},
        {9814053, "SVD(卓越)"},
        {9815001, "SKS(卓越)"},
        {9815002, "SKS(完好)"},
        {9815003, "SKS(改进)"},
        {9815004, "SKS(精制)"},
        {9815005, "SKS(卓越)"},
        {9815006, "SKS(黑鹰)"},
        {9815007, "SKS(铁爪)"},
        {9815008, "VSS(破损)"},
        {9815009, "VSS(修复)"},
        {9815010, "VSS(完好)"},
        {9815015, "Mini14(破损)"},
        {9815016, "Mini14(修复)"},
        {9815017, "Mini14(完好)"},
        {9815018, "Mini14(改进)"},
        {9815022, "Mk14(卓越)"},
        {9815023, "Mk14(完好)"},
        {9815024, "Mk14(改进)"},
        {9815025, "Mk14(精制)"},
        {9815026, "Mk14(卓越)"},
        {9815027, "Mk14(黑鹰)"},
        {9815028, "Mk14(铁爪)"},
        {9815029, "SLR(卓越)"},
        {9815030, "SLR(完好)"},
        {9815031, "SLR(改进)"},
        {9815032, "SLR(精制)"},
        {9815033, "SLR(卓越)"},
        {9815034, "SLR(黑鹰)"},
        {9815035, "SLR(铁爪)"},
        {9815036, "QBU(破损)"},
        {9815037, "QBU(修复)"},
        {9815038, "QBU(完好)"},
        {9815039, "QBU(改进)"},
        {9815043, "M417(卓越)"},
        {9815044, "M417(完好)"},
        {9815045, "M417(改进)"},
        {9815046, "M417(精制)"},
        {9815047, "M417(卓越)"},
        {9815048, "M417(黑鹰)"},
        {9815049, "M417(铁爪)"},
        {9815050, "MK20-H(卓越)"},
        {9815051, "MK20-H(完好)"},
        {9815052, "MK20-H(改进)"},
        {9815053, "MK20-H(精制)"},
        {9815054, "MK20-H(卓越)"},
        {9815055, "MK20-H(黑鹰)"},
        {9815056, "MK20-H(铁爪)"},
        {9815064, "MK12(卓越)"},
        {9815065, "MK12(完好)"},
        {9815066, "MK12(改进)"},
        {9815067, "MK12(精制)"},
        {9815068, "MK12(卓越)"},
        {9815069, "MK12(黑鹰)"},
        {9815070, "MK12(铁爪)"},
        {101008, "M762"},
        {101001, "AKM"},
        {101004, "M416"},
        {101003, "SCAR-L"},
        {101002, "M16A4"},
        {101009, "Mk47"},
        {101006, "AUG"},
        {101005, "Groza"},
        {101010, "G36C"},
        {101007, "QBZ"},
        {101011, "AC-VAL"},
        {101012, "蜜獾"},
        {103009, "SLR"},
        {103005, "VSS"},
        {103006, "Mini14"},
        {103010, "QBU"},
        {103004, "SKS"},
        {103007, "MK14"},
        {103014, "MK20-H"},
        {103013, "M417"},
        {103015, "M200"},
        {103012, "AMR"},
        {104005, "AA12"},
        {103016, "SVD"},
        {102009, "JS9"},
        {102008, "Aks-74u"},
        {103101, "电磁狙"},
        {103003, "AWM"},
        {103002, "M24"},
        {103011, "莫甘娜辛"},
        {103001, "Kar98K"},
        {103008, "Win94"},
        {105001, "M249"},
        {105002, "DP-28"},
        {105010, "MG3"},
        {107001, "十字弩"},
        {107007, "爆炸猎弓"},
        {102001, "UZI"},
        {102003, "Vector"},
        {100103, "PP-19"},
        {102007, "MP5K"},
        {102002, "UMP-45"},
        {102004, "汤姆逊"},
        {102105, "P90"},
        {102005, "野牛"},
        {104001, "S686"},
        {104002, "S1897"},
        {104003, "S12K"},
        {104004, "DBS"},
        {104100, "SPAS-12"},
        {602004, "苹果"},
        {602003, "燃烧瓶"},
        {602002, "烟雾弹"},
        {602001, "震撼弹"},
        {108003, "镰刀"},
        {108002, "撬棍"},
        {108001, "大砍刀"},
        {108004, "平底锅"},
        {0, "空手"}
    };

    auto it = gunMap.find(BoxId);
    return it != gunMap.end() ? const_cast<char*>(it->second) : const_cast<char*>("NULL");
}


inline const char* 头(int Id) {
    static const std::unordered_map<int, const char*> itemMap = {
        {502001, "头(1级)"}, {502002, "军用头盔(2级)"},
        {502003, "特种部队头盔(3级)"}, {502004, "摩托车头盔(1级)"},
        {502005, "军用头盔(2级)"}, {502006, "摩托车头盔(1级)"},
        {502007, "军用头盔(2级)"}, {502008, "特种部队头盔(3级)"},
        {502009, "特种部队头盔(4级)"}, {502010, "圣诞头盔(3级)"},
        {502102, "军用头盔(2级)"}, {502103, "守卫头盔 (2级)"},
        {502104, "指挥官头盔(3级)"}, {503103, "新春三级头(3级)"}
    };

    if (Id >= 502001000 && Id <= 502001999) return "头盔(1级)";
    if (Id >= 502002000 && Id <= 502002999) return "头盔(2级)";
    if (Id >= 502003000 && Id <= 502003999) return "头盔(3级)";

    auto it = itemMap.find(Id);
    return it != itemMap.end() ? it->second : "NULL";
}

inline const char* 甲(int Id) {
    static const std::unordered_map<int, const char*> itemMap = {
        {503001, "甲(1级)"}, {503002, "警用防弹衣(2级)"},
        {503003, "军用防弹衣(3级)"}, {503004, "警用防弹衣(1级)"},
        {503005, "警用防弹衣(2级)"}, {503006, "军用防弹衣(3级)"},
        {503007, "军用防弹衣(4级)"}, {503102, "警用防弹衣(2级)"},
        {503104, "守卫防弹衣(2级)"}, {503105, "指挥官防弹衣(3级)"}
    };

    auto it = itemMap.find(Id);
    return it != itemMap.end() ? it->second : "NULL";
}

inline const char* 包(int Id) {
    static const std::unordered_map<int, const char*> itemMap = {
        {501001, "包(1)"}, {501002, "包(2)"},
        {501003, "包(3)"}, {501004, "包(1)"},
        {501005, "包(2)"}, {501006, "包(3)"},
        {501007, "包(1)"}, {501008, "包(2)"},
        {501009, "包(3)"}
    };

    if (Id >= 501001000 && Id <= 501001999) return "包(1)";
    if (Id >= 501002000 && Id <= 501002999) return "包(2)";
    if (Id >= 501003000 && Id <= 501003390) return "包(3)";

    auto it = itemMap.find(Id);
    return it != itemMap.end() ? it->second : "NULL";
}





inline float Recoil(int WeaponId)
{
    static const std::unordered_map<int, float> recoilMap = {
        {101010, 1.30f}, {101002, 1.30f},
        {101007, 0.3f}, {101006, 0.3f}, {101005, 0.3f}, {101012, 0.3f}, {102008, 0.3f}, {101013, 0.3f},
        {101004, 1.50f}, {101011, 1.50f}, {101003, 1.50f}, {101001, 1.50f}
    };
    auto it = recoilMap.find(WeaponId);
    return it != recoilMap.end() ? it->second : 1.0f;
}


inline const std::string tou(int Id) {
    static const std::unordered_map<int, std::string> touMap = {
        {0, "NULL"},
        {9804016, "夜视仪"},
        {9804001, "头[1]"}, {502001, "头[1]"}, {502004, "头[1]"}, {502101, "头[1]"},
        {9804002, "头[2]"}, {502002, "头[2]"}, {502005, "头[2]"}, {502102, "头[2]"},
        {9804003, "头[3]"}, {502003, "头[3]"}, {502006, "头[3]"}, {502103, "头[3]"},
        {9804004, "头[4]"}, {9804005, "头[4·黑鹰]"}, {9804006, "头[4·铁爪]"}, {9804021, "头[4·轩辕]"},
        {9804007, "头[5]"}, {9804008, "头[5·黑鹰]"}, {9804009, "头[5·铁爪]"}, {9804022, "头[5·轩辕]"},
        {9804010, "头[6]"}, {9804011, "头[6·黑鹰]"}, {9804012, "头[6·铁爪]"}, {9804023, "头[6·轩辕]"},
        {9804013, "头[7]"}, {9804014, "头[7·黑鹰]"}, {9804015, "头[7·铁爪]"}, {9804016, "头[7]"},
        {9804017, "头[7·黑鹰]"}, {9804018, "头[7·铁爪]"}, {9804024, "头[7·轩辕]"}, 
        {9804025, "头[7·轩辕]"}, {9804026, "头[7·轩辕]"}
    };
    if (Id >= 502001000 && Id <= 502001999) return "头[1]";
    if (Id >= 502002000 && Id <= 502002999) return "头[2]";
    if (Id >= 502003000 && Id <= 502003999) return "头[3]";
    auto it = touMap.find(Id);
    return it != touMap.end() ? it->second : "NULL";
}

inline const std::string jia(int Id) {
    static const std::unordered_map<int, std::string> jiaMap = {
        {0, "NULL"},
        {9805001, "甲[1]"}, {503001, "甲[1]"}, {503004, "甲[1]"}, {503101, "甲[1]"},
        {9805002, "甲[2]"}, {503002, "甲[2]"}, {503005, "甲[2]"}, {503102, "甲[2]"},
        {9805003, "甲[3]"}, {503003, "甲[3]"}, {503006, "甲[3]"}, {503103, "甲[3]"},
        {9805004, "甲[4]"}, {9805005, "甲[4·铁爪]"}, {9805006, "甲[4·黑鹰]"}, {9805036, "甲[4·轩辕]"},
        {9805007, "甲[5]"}, {9805008, "甲[5·铁爪]"}, {9805009, "甲[5·黑鹰]"}, {9805037, "甲[5·轩辕]"},
        {9805010, "甲[6]"}, {9805011, "甲[6·铁爪]"}, {9805012, "甲[6·黑鹰]"}, {9805038, "甲[6·轩辕]"},
        {9805013, "甲[7]"}, {9805014, "甲[7·铁爪]"}, {9805015, "甲[7·黑鹰]"}, {9805016, "甲[7]"},
        {9805017, "甲[7·铁爪]"}, {9805018, "甲[7·黑鹰]"}, {9805039, "甲[7·轩辕]"}, {9805040, "甲[7·轩辕]"}
    };
    if (Id >= 503001000 && Id <= 503001999) return "甲[1]";
    if (Id >= 503002000 && Id <= 503002999) return "甲[2]";
    if (Id >= 503003000 && Id <= 503003999) return "甲[3]";
    auto it = jiaMap.find(Id);
    return it != jiaMap.end() ? it->second : "NULL";
}

inline const std::string bao(int Id) {
    static const std::unordered_map<int, std::string> baoMap = {
        {0, "NULL"},
        {9806001, "包[1]"}, {501001, "包[1]"}, {501004, "包[1]"}, {501101, "包[1]"},
        {9806002, "包[2]"}, {501002, "包[2]"}, {501005, "包[2]"}, {501102, "包[2]"},
        {9806003, "包[2]"}, {501003, "包[2]"}, {501006, "包[2]"}, {501103, "包[2]"},
        {9806004, "包[4]"}, {9806005, "包[4·铁爪]"}, {9806006, "包[4·黑鹰]"}, {9806019, "包[4·轩辕]"},
        {9806007, "包[5]"}, {9806008, "包[5·铁爪]"}, {9806009, "包[五.5·黑鹰]"}, {9806020, "包[5·轩辕]"},
        {9806010, "包[6]"}, {9806011, "包[6·铁爪]"}, {9806012, "包[6·黑鹰]"}, {9806021, "包[6·轩辕]"},
        {9806013, "包[7]"}, {9806014, "包[7·铁爪]"}, {9806015, "包[7·黑鹰]"}, {9806016, "包[7]"},
        {9806017, "包[7·铁爪]"}, {9806018, "包[7·黑鹰]"}, {9806022, "包[7·轩辕]"}, {9806023, "包[7·轩辕]"}
    };
    if (Id >= 501001000 && Id <= 501001999) return "包[1]";
    if (Id >= 501002000 && Id <= 501002999) return "包[2]";
    if (Id >= 501003000 && Id <= 501003390) return "包[2]";

    auto it = baoMap.find(Id);
    return it != baoMap.end() ? it->second : "NULL";
}

inline float GetWeaponId(int WeaponId)
{
    static const std::unordered_map<int, float> weaponMap = {
        {102105, 0.30f}, {101009, 0.30f}, {105010, 0.30f}, {105002, 0.30f},
        {103007, 0.55f}, {102008, 0.55f},
        {103005, 0.40f}, {102007, 0.40f}, {102004, 0.40f}, {102003, 0.40f}, {102002, 0.40f}, {102001, 0.40f}, {106008, 0.40f}, {106011, 0.40f}, {105012, 0.40f}, {102009, 0.40f},
        {102005, 0.45f},
        {101001, 0.95f}, {106004, 0.95f},
        {101008, 0.85f}, {101005, 0.85f}, {101012, 0.85f},
        {101006, 0.65f}, {101004, 0.65f}, {101007, 0.65f}, {101003, 0.65f}, {105001, 0.65f},
        {101011, 0.40f}, {101002, 0.40f},
        {101013, 0.34f},
        {101010, 0.60f}
    };
    auto it = weaponMap.find(WeaponId);
    return it != weaponMap.end() ? it->second : 0.20f;
}

inline float getScopeAcc_Gyro_High(float Fov)
{
    if(Fov > 75)
    {
        return 1.2f;
    }
    int Scope = 90/Fov;
    static const std::unordered_map<int, float> scopeMap = {
        {0, .6f},
        {1, .6f},    //红点
        {2, .36f},
        {3, .27f},
        {4, .17f},
        {5, .15f},
        {6, .14f},
        {7, .13f},
        {8, .12f}
    };
    auto it = scopeMap.find(Scope);
    return it != scopeMap.end() ? it->second : .15f;
}
inline float getScopeAcc(int Scope)
{
    static const std::unordered_map<int, float> scopeMap = {
        {0, 1.0f},
        {1, .98f},
        {2, .95f},
        {3, .94f},
        {4, .92f},
        {5, .9f},
        {6, .88f},
        {7, .86f},
        {8, .85f}
    };
    auto it = scopeMap.find(Scope);
    return it != scopeMap.end() ? it->second : .9f;
}

inline const std::string 超体职业(int Id) {
    static const std::unordered_map<int, std::string> classMap = {
        {501003994, "指挥官"},
        {501003995, "火箭小子"},
        {501003996, "次元极客"},
        {501003997, "战地医师"},
        {501003998, "爆破上尉"},
        {501003999, "闪电尖兵"},
        {501004000, "炫彩机师"},
        {501004001, "幻影猎手"},
        {501004002, "时空魅影"},
        {501004003, "雷枪少年"},
        {501004004, "涂鸦风暴"},
        {501004005, "海豚舞者"},
        {501004006, "孤胆枪手"},
        {501004007, "源代码"},
        {501004008, "天籁之音"},
        {501004009, "爆裂萌兔"},
        {5305001, "野蛮人"},
        {5305002, "游侠"},
        {5305003, "法师"},
        {5305004, "圣骑士"}
    };
    if (Id > 5305000 && Id < 5305005) {
        auto it = classMap.find(Id);
        if (it != classMap.end()) {
            return it->second;
        }
    }

    if (Id >= 501003994 && Id <= 501004009) {
        auto it = classMap.find(Id);
        if (it != classMap.end()) {
            return it->second;
        }
    }
    return "NULL";
}



inline const char *GetHol(int dz)
{
    static const std::unordered_map<int, const char *> holMap = {
        {655360, "自救"},
        {4112, "探头"}, {4128, "探头"},
        {2064, "拉栓"}, {2065, "拉栓"}, {2067, "拉栓"}, {2080, "拉栓"}, {2081, "拉栓"}, {2083, "拉栓"}, {2113, "拉栓"}, {2115, "拉栓"}, {6160, "拉栓"}, {6176, "拉栓"},
        {288, "开火"}, {273, "开火"}, {320, "开火"}, {4384, "开火"},
        {33, "行走"}, {17, "行走"}, {65, "行走"},
        {4194320, "坐车"}, {1048576, "坐船"}, {1050624, "坐船"},
        {67108880, "翻墙"},
        {16, "站立"},
        {144, "跳跃"}, {147, "跳跃"}, {145, "跳跃"},
        {33554448, "跳伞"},
        {32, "蹲着"}, {64, "趴着"},
        {6552, "掐雷"},
        {19, "奔跑"}, {35, "奔跑"},
        {272, "开枪"},
        {1040, "开镜"}, {1041, "开镜"}, {1056, "开镜"}, {1057, "开镜"}, {1088, "开镜"}, {5136, "开镜"}, {5137, "开镜"}, {5152, "开镜"}, {5153, "开镜"},
        {2097168, "开车"},
        {67, "爬行"},
        {536870928, "跳舞"},
        {528, "换弹"}, {544, "换弹"}, {529, "换弹"}, {545, "换弹"}, {576, "换弹"},
        {131072, "倒地"}, {524303, "倒地"}, {524304, "倒地"}, {524289, "倒地"}, {524288, "倒地"},
        {32784, "挥拳"}, {32785, "挥拳"}, {32787, "挥拳"}, {32800, "挥拳"}, {32801, "挥拳"}, {32803, "挥拳"}, {32912, "挥拳"}, {32913, "挥拳"}, {32915, "挥拳"},
        {8208, "切枪"},
        {16777219, "游泳"}, {16777216, "游泳"}, {16777217, "游泳"},
        {65540, "打药"}, {65541, "打药"}, {65542, "打药"}, {65543, "打药"}, {65544, "打药"}, {65545, "打药"}, {65546, "打药"}, {65547, "打药"}, {65548, "打药"}, {65549, "打药"}, {65550, "打药"}, {65551, "打药"}, {65552, "打药"}, {65553, "打药"}, {65554, "打药"}, {65555, "打药"}, {65556, "打药"}, {65557, "打药"}, {65558, "打药"}, {65559, "打药"}, {65560, "打药"}, {65561, "打药"}, {65562, "打药"}, {65563, "打药"}, {65564, "打药"}, {65565, "打药"}, {65566, "打药"}, {65567, "打药"}, {65568, "打药"}, {65569, "打药"}, {65570, "打药"}, {65571, "打药"}, {65572, "打药"}, {65573, "打药"}, {65574, "打药"}, {65575, "打药"}, {65576, "打药"}, {65577, "打药"}, {65578, "打药"}, {65579, "打药"}, {65580, "打药"},
        {262161, "打药"}, {262160, "打药"}, {262176, "打药"}, {262177, "打药"}, {262208, "打药"}, {262209, "打药"},
        {16390, "投掷"}, {16391, "投掷"}, {16392, "投掷"}, {16393, "投掷"}, {16394, "投掷"}, {16395, "投掷"}, {16396, "投掷"}, {16397, "投掷"}, {16398, "投掷"}, {16399, "投掷"}, {16400, "投掷"}, {16401, "投掷"}, {16402, "投掷"}, {16403, "投掷"}, {16404, "投掷"}, {16405, "投掷"}, {16406, "投掷"}, {16407, "投掷"}, {16408, "投掷"}, {16409, "投掷"}, {16410, "投掷"}, {16411, "投掷"}, {16412, "投掷"}, {16413, "投掷"}, {16414, "投掷"}, {16415, "投掷"},
        {17410, "投掷"}, {17411, "投掷"}, {17412, "投掷"}, {17413, "投掷"}, {17414, "投掷"}, {17415, "投掷"}, {17416, "投掷"}, {17417, "投掷"}, {17418, "投掷"}, {17419, "投掷"}, {17420, "投掷"}, {17421, "投掷"}, {17422, "投掷"}, {17423, "投掷"}, {17424, "投掷"}, {17425, "投掷"}, {17426, "投掷"}, {17427, "投掷"}, {17428, "投掷"}, {17429, "投掷"}, {17430, "投掷"}
    };
    auto it = holMap.find(dz);
    return it != holMap.end() ? it->second : "";
}



inline const char *Getagrenade(int value)
{
    static const std::unordered_map<int, const char*> grenadeMap = {
        {602001, "震爆来了"}, {9825001, "震爆来了"},
        {602003, "燃烧瓶来了"}, {9825003, "燃烧瓶来了"},
        {602004, "手雷来了"}, {9825004, "手雷来了"},
        {602075, "铝热弹来了"}
    };

    auto it = grenadeMap.find(value);
    return it != grenadeMap.end() ? it->second : nullptr;
}
