#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <cmath>
#include <linux/input.h>
#include <linux/uinput.h>
#include <cstdio>
#include <cstdlib>
#include <random>
#include "辅助类.h"
#include "imgui.h"
#include <vector>
#include <thread>
#include <unordered_map>
#include "spinlock.h"
#define maxE 5
#define maxF 10
#define UNGRAB 0
#define GRAB 1

using namespace std;

namespace TouchConfig {
    // 默认随机范围：1.5~2次/秒（对应间隔500ms~666ms）
    static std::chrono::milliseconds minIntervalMs(500);
    static std::chrono::milliseconds maxIntervalMs(666);
    static std::mt19937 rng(std::random_device{}()); // 线程安全的随机数生成器
    // 设置频率范围（转换为时间间隔）
    void setClickFrequencyRange(float minClicksPerSec, float maxClicksPerSec) {
        if(minClicksPerSec > 0 && maxClicksPerSec > minClicksPerSec) {
            minIntervalMs = std::chrono::milliseconds(static_cast<int>(1000 / maxClicksPerSec));
            maxIntervalMs = std::chrono::milliseconds(static_cast<int>(1000 / minClicksPerSec));
        }
    }
    // 生成随机间隔
    std::chrono::milliseconds getRandomInterval() {
        std::uniform_int_distribution<int> dist(
            minIntervalMs.count(), 
            maxIntervalMs.count()
        );
        return std::chrono::milliseconds(dist(rng));
    }
}
    
extern 绘制 绘制;

bool other_touch;

static uint32_t orientation = 0;
static float screenHeight = 0, screenWidth = 0;

struct touchObj
{
    int id = 0;
    int slot = 0;
    bool isDown = false;
    bool isTmpDown = false;
    int x = 0;
    int y = 0;
    int size1 = 0;
    int size2 = 0;
    int size3 = 0;
};
struct Device {
        int fd;
        float S2TX;
        float S2TY;
        input_absinfo absX, absY;
        touchObj Finger[10];

        Device() { memset((void *) this, 0, sizeof(*this)); }
};
struct targ
{
    int fdNum;
    float S2TX;
    float S2TY;
};
static struct
{
    input_event downEvent[2]{{{}, EV_KEY, BTN_TOUCH, 1},
                             {{}, EV_KEY, BTN_TOOL_FINGER, 1}};
    input_event event[512]{0};
} input;

static targ targF[maxE];
static spinlock spin_lock;
static touchObj Finger[maxE][maxF];

static int fdNum = 0, origfd[maxE], nowfd;

static float scale_x, scale_y;

static bool Touch_initialized = false;

static bool Touch_readOnly = false;

static std::vector<Device> devices;

static bool checkDeviceIsTouch(int fd);
static void genRandomString(char *string, int length)
{
    int flag, i;
    srand((unsigned)time(NULL) + length);
    for (i = 0; i < length - 1; i++)
    {
        flag = rand() % 3;
        switch (flag)
        {
        case 0:
            string[i] = 'A' + rand() % 26;
            break;
        case 1:
            string[i] = 'a' + rand() % 26;
            break;
        case 2:
            string[i] = '0' + rand() % 10;
            break;
        default:
            string[i] = 'x';
            break;
        }
    }
    string[length - 1] = '\0';
}
/**
 * @brief
 * 转屏幕坐标
 * @param x
 * @param y
 * @return Vector2
 */
D2DVector Touch2Screen(float x, float y)
{
    
    float xt = x / scale_x;
    float yt = y / scale_y;
    if (other_touch)
    {
        switch (orientation)
        {
        case 1:
            x = xt;
            y = yt;
            break;
        case 2:
            y = yt;
            x = screenHeight - xt;
            break;
        case 3:
            x = screenHeight - xt;
            y = screenWidth - yt;
            break;
        default:
            y = xt;
            x = screenHeight - yt;
            break;
        }
    }
    else
    {
        switch (orientation)
        {
        case 1:
            x = yt;
            y = screenHeight - xt;
            break;
        case 2:
            x = screenHeight - xt;
            y = screenWidth - yt;
            break;
        case 3:
            y = xt;
            x = screenWidth - yt;
            break;
        default:
            x = xt;
            y = yt;
            break;
        }
    }
    return {x, y};
}


static void Upload() {
        static bool isFirstDown = true;
        int tmpCnt = 0, tmpCnt2 = 0;
        for (auto& device : devices) {
            for (auto& finger : device.Finger) {
                if (finger.isDown) {
                    if (tmpCnt2++ > 20) {
                        goto finish;
                    }
                    input.event[tmpCnt].type = EV_ABS;
                    input.event[tmpCnt].code = ABS_X;
                    input.event[tmpCnt].value = (int)finger.x;
                    tmpCnt++;

                    input.event[tmpCnt].type = EV_ABS;
                    input.event[tmpCnt].code = ABS_Y;
                    input.event[tmpCnt].value = (int)finger.y;
                    tmpCnt++;

                    input.event[tmpCnt].type = EV_ABS;
                    input.event[tmpCnt].code = ABS_MT_POSITION_X;
                    input.event[tmpCnt].value = (int)finger.x;
                    tmpCnt++;

                    input.event[tmpCnt].type = EV_ABS;
                    input.event[tmpCnt].code = ABS_MT_POSITION_Y;
                    input.event[tmpCnt].value = (int)finger.y;
                    tmpCnt++;

                    input.event[tmpCnt].type = EV_ABS;
                    input.event[tmpCnt].code = ABS_MT_TRACKING_ID;
                    input.event[tmpCnt].value = finger.id;
                    tmpCnt++;
                    // 多指上报
                    input.event[tmpCnt].type = EV_ABS;
                    input.event[tmpCnt].code = ABS_MT_SLOT;
                    input.event[tmpCnt].value = finger.id;
                    tmpCnt++;

                    input.event[tmpCnt].type = EV_SYN;
                    input.event[tmpCnt].code = SYN_MT_REPORT;
                    input.event[tmpCnt].value = 0;
                    tmpCnt++;
                }
            }
        }
    finish:
        bool is = false;
        if (tmpCnt == 0) {
            input.event[tmpCnt].type = EV_SYN;
            input.event[tmpCnt].code = SYN_MT_REPORT;
            input.event[tmpCnt].value = 0;
            tmpCnt++;
            if (!isFirstDown) {
                isFirstDown = true;
                input.event[tmpCnt].type = EV_KEY;
                input.event[tmpCnt].code = BTN_TOUCH;
                input.event[tmpCnt].value = 0;
                tmpCnt++;
                input.event[tmpCnt].type = EV_KEY;
                input.event[tmpCnt].code = BTN_TOOL_FINGER;
                input.event[tmpCnt].value = 0;
                tmpCnt++;
            }
        }
        else {
            is = true;
        }
        input.event[tmpCnt].type = EV_SYN;
        input.event[tmpCnt].code = SYN_REPORT;
        input.event[tmpCnt].value = 0;
        tmpCnt++;

        if (is && isFirstDown) {
            isFirstDown = false;
            write(nowfd, &input, sizeof(struct input_event) * (tmpCnt + 2));
        }
        else {
            write(nowfd, input.event, sizeof(struct input_event) * tmpCnt);
        }
    }
    
static void* TypeA(void* arg) {
        int i = (int)(long)arg;
        Device& device = devices[i];

        int latest = 0;
        input_event inputEvent[64]{ 0 };
        timer touchFPS;
        touchFPS.SetFps(800);
        touchFPS.AotuFPS_init();
        touchFPS.setAffinity();
        while (Touch_initialized) {
            auto readSize = (int32_t)read(device.fd, inputEvent, sizeof(inputEvent));
            if (readSize <= 0 || (readSize % sizeof(input_event)) != 0) {
                continue;
            }
            size_t count = size_t(readSize) / sizeof(input_event);

            spin_lock.lock();
            for (size_t j = 0; j < count; j++) {
                input_event& ie = inputEvent[j];
                if (ie.type == EV_ABS) {
                    if (ie.code == ABS_MT_SLOT) {
                        latest = ie.value;
                        continue;
                    }
                    if (ie.code == ABS_MT_TRACKING_ID) {
                        if (ie.value == -1) {
                            device.Finger[latest].isDown = false;
                        }
                        else {
                            device.Finger[latest].id = (i * 2 + 1) * maxF + latest;
                            device.Finger[latest].isDown = true;
                        }
                        continue;
                    }
                    if (ie.code == ABS_MT_POSITION_X) {
                        device.Finger[latest].id = (i * 2 + 1) * maxF + latest;
                        device.Finger[latest].x = (float)ie.value * device.S2TX;
                        continue;
                    }
                    if (ie.code == ABS_MT_POSITION_Y) {
                        device.Finger[latest].id = (i * 2 + 1) * maxF + latest;
                        device.Finger[latest].y = (float)ie.value * device.S2TY;
                        continue;
                    }
                }
                if (ie.code == SYN_REPORT) {
                    if (ImGui::GetCurrentContext() != nullptr) {
                        ImGuiIO& io = ImGui::GetIO();
                        if (device.Finger[latest].isDown) {
                            auto pos = Touch2Screen(device.Finger[latest].x, device.Finger[latest].y);
                            io.MousePos = ImVec2(pos.X, pos.Y);
                            io.MouseDown[0] = true;
                        }
                        else {
                            io.MouseDown[0] = false;
                        }
                    }

                    if (!Touch_readOnly) {
                        Upload();
                    }
                    continue;
                }
            }
            spin_lock.unlock();
            touchFPS.SetFps((int)绘制.自瞄.触摸采样率);
            touchFPS.AotuFPS();
        }
        return nullptr;
    }

bool Touch_Init(int w, int h, uint32_t orientation_, bool readOnly)
{

    char temp[128];
    DIR *dir = opendir("/dev/input/");
    dirent *ptr = NULL;
    int eventCount = 0;
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strstr(ptr->d_name, "event"))
            eventCount++;
    }
    struct input_absinfo abs, absX[maxE], absY[maxE];
    int fd, i, tmp1, tmp2;
    int screenX, screenY, minCnt = eventCount + 1;
    fdNum = 0;
    for (i = 0; i <= eventCount; i++)
    {
        sprintf(temp, "/dev/input/event%d", i);
        fd = open(temp, O_RDWR);
        if (fd < 0)
        {
            continue;
        }
        if (checkDeviceIsTouch(fd))
        {
            tmp1 = ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &absX[fdNum]);
            tmp2 = ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &absY[fdNum]);
            if (tmp1 == 0 && tmp2 == 0)
            {
                origfd[fdNum] = fd;
                Device device{};
                if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &device.absX) == 0
                    && ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &device.absY) == 0) {
                    device.fd = fd;
                    if (!readOnly) {
                        ioctl(fd, EVIOCGRAB, GRAB);
                    }
                    devices.push_back(device);
                }
                if (i < minCnt)
                {
                    screenX = absX[fdNum].maximum;
                    screenY = absY[fdNum].maximum;
                    minCnt = i;
                }
                fdNum++;
                if (fdNum >= maxE)
                    break;
            }
        }
        else
        {
            close(fd);
        }
    }

    if (minCnt > eventCount)
    {
        puts("获取屏幕驱动失败");
        return false;
    }

    if (!readOnly)
    {

        struct uinput_user_dev ui_dev;
        nowfd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (nowfd < 0)
        {
            perror("open /dev/uinput");
            return false;
        }

        memset(&ui_dev, 0, sizeof(ui_dev));
        strncpy(ui_dev.name, "Virtual Touchscreen", sizeof(ui_dev.name));
        ui_dev.id.bustype = BUS_VIRTUAL;
        ui_dev.id.vendor = 0x1234;
        ui_dev.id.product = 0x5678;
        ui_dev.id.version = 1;

        if (ioctl(nowfd, UI_SET_PROPBIT, INPUT_PROP_DIRECT) == -1 ||
            ioctl(nowfd, UI_SET_EVBIT, EV_ABS) == -1 ||
            ioctl(nowfd, UI_SET_ABSBIT, ABS_X) == -1 ||
            ioctl(nowfd, UI_SET_ABSBIT, ABS_Y) == -1 ||
            ioctl(nowfd, UI_SET_ABSBIT, ABS_MT_POSITION_X) == -1 ||
            ioctl(nowfd, UI_SET_ABSBIT, ABS_MT_POSITION_Y) == -1 ||
            ioctl(nowfd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID) == -1 ||
            ioctl(nowfd, UI_SET_EVBIT, EV_SYN) == -1 ||
            ioctl(nowfd, UI_SET_EVBIT, EV_KEY) == -1 ||
            ioctl(nowfd, UI_SET_KEYBIT, BTN_TOOL_FINGER) == -1 ||
            ioctl(nowfd, UI_SET_KEYBIT, BTN_TOUCH) == -1)
        {
            perror("Failed to set uinput properties");
            close(nowfd);
            return false;
        }

        ui_dev.absmin[ABS_MT_POSITION_X] = 0;
        ui_dev.absmax[ABS_MT_POSITION_X] = screenX;
        ui_dev.absmin[ABS_MT_POSITION_Y] = 0;
        ui_dev.absmax[ABS_MT_POSITION_Y] = screenY;
        ui_dev.absmin[ABS_X] = 0;
        ui_dev.absmax[ABS_X] = screenX;
        ui_dev.absmin[ABS_Y] = 0;
        ui_dev.absmax[ABS_Y] = screenY;
        ui_dev.absmin[ABS_MT_TRACKING_ID] = 0;
        ui_dev.absmax[ABS_MT_TRACKING_ID] = 65535;

        if (write(nowfd, &ui_dev, sizeof(ui_dev)) == -1)
        {
            perror("Failed to write uinput device");
            close(nowfd);
            return false;
        }
        if (ioctl(nowfd, UI_DEV_CREATE) == -1)
        {
            perror("Failed to create uinput device");
            close(nowfd);
            return false;
        }
    }

    Touch_initialized = true;
    Touch_readOnly = readOnly;

    pthread_t t;
        for (int i = 0; i < devices.size(); i++) {
            devices[i].S2TX = (float)screenX / (float)devices[i].absX.maximum;
            devices[i].S2TY = (float)screenY / (float)devices[i].absY.maximum;
            pthread_create(&t, nullptr, TypeA, (void*)(long)i);
        }
    fdNum++;
    ::screenWidth = w;
    ::screenHeight = h,
    ::orientation = orientation_;
    if (::orientation == 1 || ::orientation == 3)
    {
        ::scale_x = (float)screenX / h;
        ::scale_y = (float)screenY / w;
    }
    else
    {
        ::scale_x = (float)screenX / w;
        ::scale_y = (float)screenY / h;
    }
    system("chmod 000 -R /proc/bus/input/*");
    return true;
}

void UpdateScreenData(int w, int h, uint32_t orientation_)
{
    ::screenWidth = w;
    ::screenHeight = h,
    ::orientation = orientation_;
}
static bool checkDeviceIsTouch(int fd)
{
    uint8_t *bits = NULL;
    ssize_t bits_size = 0;
    int res, j, k;
    bool itmp = false, itmp2 = false, itmp3 = false;
    struct input_absinfo abs{};
    while (true)
    {
        res = ioctl(fd, EVIOCGBIT(EV_ABS, bits_size), bits);
        if (res < bits_size)
            break;
        bits_size = res + 16;
        bits = (uint8_t *)realloc(bits, bits_size * 2);
    }
    for (j = 0; j < res; j++)
    {
        for (k = 0; k < 8; k++)
            if (bits[j] & 1 << k && ioctl(fd, EVIOCGABS(j * 8 + k), &abs) == 0)
            {
                if (j * 8 + k == ABS_MT_SLOT)
                {
                    itmp = true;
                    continue;
                }
                if (j * 8 + k == ABS_MT_POSITION_X)
                {
                    itmp2 = true;
                    continue;
                }
                if (j * 8 + k == ABS_MT_POSITION_Y)
                {
                    itmp3 = true;
                    continue;
                }
            }
    }
    free(bits);
    return itmp && itmp2 && itmp3;
}

void Touch_Down(float x, float y)
{
    spin_lock.lock();
    touchObj &touch = devices[0].Finger[9];
    touch.id = 8;
    touch.x = (int)(x * scale_x);
    touch.y = (int)(y * scale_y);
    touch.isDown = true;
    touch.size1 = 8;
    touch.size2 = 8;
    touch.size3 = 8;
    Upload();
    spin_lock.unlock();
}

void Touch_Down(int id, float x, float y)
{
    spin_lock.lock();
    touchObj &touch = devices[0].Finger[id];
    touch.id = id;
    touch.x = (int)(x * scale_x);
    touch.y = (int)(y * scale_y);
    touch.isDown = true;
    Upload();
    spin_lock.unlock();
}

void Touch_Up(int id)
{
    spin_lock.lock();
    touchObj &touch = devices[0].Finger[id];
    touch.isDown = false;
    Upload();
    spin_lock.unlock();
}

void Touch_Move(float x, float y)
{
    Touch_Down(x, y);
}
void Touch_Up()
{
    spin_lock.lock();
    touchObj &touch = devices[0].Finger[9];
    touch.isDown = false;
    Upload();
    spin_lock.unlock();
}
/**
 * @brief 
 * 
 * @param isClick 如果在矩形内是否需要连点
 * @param clickRect 连点矩形位置
 * @return true 手指在连点位置内
 * @return false 手指不在连点位置内
 */
bool clickRegion(bool isClick, Rect clickRect) {
    static std::chrono::steady_clock::time_point lastClickTime;
    // 连点频率， 每秒4-5次点击速度
    TouchConfig::setClickFrequencyRange(4.0f*绘制.自瞄.连点速度, 5.0f*绘制.自瞄.连点速度);

    for (auto& device : devices) {
        for (auto& finger : device.Finger) {
            if (finger.isDown) {
                auto pos = Touch2Screen(finger.x, finger.y);
                float mouseX = pos.X;
                float mouseY = pos.Y;
                if (clickRect.contains(mouseX, mouseY))
                {
                    spin_lock.lock();
                    auto now = std::chrono::steady_clock::now();
                    // 动态生成随机间隔
                    auto requiredInterval = TouchConfig::getRandomInterval();
                    if (now - lastClickTime >= requiredInterval && isClick)
                    {
                        //printf("等待sx: %d, sy: %d, ex: %d, ey: %d .........\n", clickRect.startX, clickRect.startY, clickRect.endX, clickRect.endY);
                        // 抬起用户连点位置手指
                        finger.isDown = false;
                        Upload();
                        finger.isDown = true;
                        lastClickTime = now;
                        //printf("触发连点\n");
                    }
                    spin_lock.unlock();
                    return true;
                }
            }
        }
    }
    return false;
}
