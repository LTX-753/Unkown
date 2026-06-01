from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    BaseDocTemplate,
    Frame,
    PageBreak,
    PageTemplate,
    Paragraph,
    Preformatted,
    Spacer,
    Table,
    TableStyle,
)


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs" / "数字人旋钮设备通信协议说明.pdf"
FONT_NORMAL = r"C:\Windows\Fonts\simhei.ttf"


pdfmetrics.registerFont(TTFont("CN", FONT_NORMAL))
pdfmetrics.registerFont(TTFont("CN-Bold", FONT_NORMAL))


def on_page(canvas, doc):
    canvas.saveState()
    canvas.setFont("CN", 8)
    canvas.setFillColor(colors.HexColor("#6B7280"))
    canvas.drawString(18 * mm, 12 * mm, "UEDX48480021-MD80ET 数字人旋钮设备通信协议")
    canvas.drawRightString(192 * mm, 12 * mm, f"第 {doc.page} 页")
    canvas.restoreState()


def p(text, style):
    return Paragraph(text, style)


def table(data, widths):
    t = Table(data, colWidths=widths, hAlign="LEFT")
    t.setStyle(
        TableStyle(
            [
                ("FONTNAME", (0, 0), (-1, -1), "CN"),
                ("FONTNAME", (0, 0), (-1, 0), "CN-Bold"),
                ("FONTSIZE", (0, 0), (-1, -1), 9),
                ("TEXTCOLOR", (0, 0), (-1, 0), colors.white),
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#2563EB")),
                ("BACKGROUND", (0, 1), (-1, -1), colors.HexColor("#F8FAFC")),
                ("GRID", (0, 0), (-1, -1), 0.4, colors.HexColor("#CBD5E1")),
                ("VALIGN", (0, 0), (-1, -1), "TOP"),
                ("LEFTPADDING", (0, 0), (-1, -1), 6),
                ("RIGHTPADDING", (0, 0), (-1, -1), 6),
                ("TOPPADDING", (0, 0), (-1, -1), 5),
                ("BOTTOMPADDING", (0, 0), (-1, -1), 5),
            ]
        )
    )
    return t


styles = getSampleStyleSheet()
styles.add(
    ParagraphStyle(
        name="TitleCN",
        parent=styles["Title"],
        fontName="CN-Bold",
        fontSize=22,
        leading=30,
        alignment=TA_CENTER,
        textColor=colors.HexColor("#111827"),
        spaceAfter=8,
    )
)
styles.add(
    ParagraphStyle(
        name="SubtitleCN",
        parent=styles["Normal"],
        fontName="CN",
        fontSize=10,
        leading=16,
        alignment=TA_CENTER,
        textColor=colors.HexColor("#4B5563"),
        spaceAfter=16,
    )
)
styles.add(
    ParagraphStyle(
        name="H1CN",
        parent=styles["Heading1"],
        fontName="CN-Bold",
        fontSize=15,
        leading=22,
        textColor=colors.HexColor("#1F2937"),
        spaceBefore=10,
        spaceAfter=8,
    )
)
styles.add(
    ParagraphStyle(
        name="BodyCN",
        parent=styles["BodyText"],
        fontName="CN",
        fontSize=9.5,
        leading=15,
        alignment=TA_LEFT,
        textColor=colors.HexColor("#111827"),
        spaceAfter=7,
    )
)
styles.add(
    ParagraphStyle(
        name="CodeCN",
        fontName="CN",
        fontSize=8,
        leading=11,
        leftIndent=0,
        rightIndent=0,
        textColor=colors.HexColor("#111827"),
        backColor=colors.HexColor("#F3F4F6"),
        borderPadding=6,
        spaceBefore=3,
        spaceAfter=8,
    )
)


doc = BaseDocTemplate(
    str(OUT),
    pagesize=A4,
    leftMargin=18 * mm,
    rightMargin=18 * mm,
    topMargin=18 * mm,
    bottomMargin=20 * mm,
)
frame = Frame(doc.leftMargin, doc.bottomMargin, doc.width, doc.height, id="normal")
doc.addPageTemplates([PageTemplate(id="main", frames=[frame], onPage=on_page)])

story = []

story.append(p("数字人旋钮设备通信协议说明", styles["TitleCN"]))
story.append(p("适用于 C# 数字人切换程序对接 | 版本 v0.2 | 2026-06-01", styles["SubtitleCN"]))

story.append(p("1. 设备概览", styles["H1CN"]))
story.append(
    p(
        "该设备当前被改造为数字人形象选择控制器。设备端负责显示当前数字人形象，并把旋钮、按键、触摸事件通过 USB 串口输出为一行一个 JSON，供上位机或 C# 程序实时接收。",
        styles["BodyCN"],
    )
)
story.append(
    table(
        [
            ["项目", "内容"],
            ["设备型号", "UEDX48480021-MD80ET"],
            ["主控芯片", "ESP32-S3 QFN56, revision v0.2"],
            ["无线能力", "WiFi / BLE"],
            ["Flash / PSRAM", "16MB Flash / 8MB Embedded PSRAM"],
            ["显示屏", "2.1 英寸 IPS, 480 x 480"],
            ["触摸芯片", "CST826, I2C"],
            ["USB 识别", "VID:PID = 303A:1001, USB 串行设备 / USB JTAG serial debug unit"],
            ["当前测试端口", "COM3"],
            ["设备 MAC", "20:6e:f1:b8:e7:1c"],
            ["安全状态", "Secure Boot Disabled, Flash Encryption Disabled"],
        ],
        [42 * mm, 124 * mm],
    )
)

story.append(p("2. 硬件输入与人物映射", styles["H1CN"]))
story.append(
    table(
        [
            ["输入来源", "硬件/引脚", "当前动作"],
            ["旋钮左转", "PHA=IO6, PHB=IO5", "切换到上一个数字人，并输出 knob/left 事件"],
            ["旋钮右转", "PHA=IO6, PHB=IO5", "切换到下一个数字人，并输出 knob/right 事件"],
            ["旋钮按压", "GPIO0 / SWKEY", "长按输出 button/long_press_start；短按预留/需继续调试"],
            ["触摸点击", "CST826 触摸屏", "切换到下一个数字人，并输出 touch/click 事件"],
        ],
        [36 * mm, 48 * mm, 82 * mm],
    )
)
story.append(
    table(
        [
            ["avatar", "name", "中文形象"],
            ["0", "curie", "居里夫人"],
            ["1", "einstein", "爱因斯坦"],
            ["2", "confucius", "孔子"],
            ["3", "newton", "牛顿"],
            ["4", "sushi", "苏轼"],
        ],
        [28 * mm, 54 * mm, 84 * mm],
    )
)

story.append(p("3. 串口连接参数", styles["H1CN"]))
story.append(
    table(
        [
            ["参数", "值"],
            ["端口", "Windows 下当前为 COM3，实际部署时以设备管理器或程序枚举结果为准"],
            ["波特率", "115200"],
            ["数据位", "8"],
            ["校验位", "None"],
            ["停止位", "1"],
            ["换行", "每条 JSON 以 \\n 结束；C# 可用 ReadLine 按行读取"],
            ["占用限制", "同一时间只能有一个程序打开 COM 口；调试脚本、烧录工具、C# 程序不能同时占用"],
        ],
        [42 * mm, 124 * mm],
    )
)

story.append(p("4. 设备输出 JSON 协议", styles["H1CN"]))
story.append(
    p(
        "设备输出是一行一个 JSON。C# 程序只需要按行读取并反序列化，不建议按固定字节长度读取。字段 seq 和 ms 已加入，用于排查丢包、乱序、重复触发与响应延迟。",
        styles["BodyCN"],
    )
)
story.append(
    table(
        [
            ["字段", "类型", "说明"],
            ["type", "string", "消息类型：device 表示设备状态，input 表示用户输入事件"],
            ["seq", "uint32", "设备启动后的消息自增序号。每发一条消息递增 1"],
            ["ms", "uint32", "设备启动后的毫秒时间戳，来自 millis()"],
            ["source", "string", "输入来源：knob / button / touch / serial。input 消息存在"],
            ["action", "string", "动作：left / right / click / long_press_start / next / prev / select 等"],
            ["value", "int", "原始事件值或命令参数，便于底层调试"],
            ["avatar", "int", "设备当前数字人索引，0 到 4"],
            ["name", "string", "设备当前数字人英文 ID"],
            ["event", "string", "设备状态事件：ready / pong / status。device 消息存在"],
        ],
        [26 * mm, 24 * mm, 116 * mm],
    )
)

story.append(p("设备启动/连接状态示例：", styles["BodyCN"]))
story.append(
    Preformatted(
        '{"type":"device","seq":1,"ms":15120,"event":"ready","model":"UEDX48480021-MD80ET","avatar":0,"baud":115200}',
        styles["CodeCN"],
    )
)
story.append(p("硬件输入事件示例：", styles["BodyCN"]))
story.append(
    Preformatted(
        '\n'.join(
            [
                '{"type":"input","seq":6,"ms":21034,"source":"knob","action":"left","value":0,"avatar":3,"name":"newton"}',
                '{"type":"input","seq":7,"ms":21892,"source":"knob","action":"right","value":1,"avatar":4,"name":"sushi"}',
                '{"type":"input","seq":8,"ms":23510,"source":"touch","action":"click","value":0,"avatar":0,"name":"curie"}',
                '{"type":"input","seq":9,"ms":25143,"source":"button","action":"long_press_start","value":7,"avatar":4,"name":"sushi"}',
            ]
        ),
        styles["CodeCN"],
    )
)

story.append(PageBreak())
story.append(p("5. 上位机可发送的调试命令", styles["H1CN"]))
story.append(
    p(
        "当前阶段业务命令尚未定型，设备只保留少量调试/适配命令，便于 C# 侧确认串口链路、同步状态和验证形象切换。",
        styles["BodyCN"],
    )
)
story.append(
    table(
        [
            ["命令", "设备响应/效果"],
            ["ping", "返回 device/pong，用于确认串口链路存活"],
            ["status", "返回 device/status，包含当前 avatar"],
            ["next", "切换到下一个数字人，并输出 source=serial, action=next 的 input 消息"],
            ["prev", "切换到上一个数字人，并输出 source=serial, action=prev 的 input 消息"],
            ["select:N", "切换到指定索引 N，范围建议为 0 到 4"],
        ],
        [42 * mm, 124 * mm],
    )
)
story.append(p("命令交互示例：", styles["BodyCN"]))
story.append(
    Preformatted(
        '\n'.join(
            [
                "PC -> Device: ping",
                'Device -> PC: {"type":"device","seq":2,"ms":15783,"event":"pong","model":"UEDX48480021-MD80ET","avatar":0,"baud":115200}',
                "PC -> Device: next",
                'Device -> PC: {"type":"input","seq":4,"ms":16484,"source":"serial","action":"next","value":0,"avatar":1,"name":"einstein"}',
            ]
        ),
        styles["CodeCN"],
    )
)

story.append(p("6. C# 接入建议", styles["H1CN"]))
story.append(
    p(
        "C# 程序建议使用 SerialPort 按行读取，并在非 UI 线程中解析 JSON。收到 input 消息后，优先用 avatar 字段同步人物索引；source/action 可用于区分旋钮、触摸、按键来源。",
        styles["BodyCN"],
    )
)
story.append(
    Preformatted(
        r'''using System;
using System.IO.Ports;
using System.Text.Json;

var port = new SerialPort("COM3", 115200)
{
    NewLine = "\n",
    ReadTimeout = 100,
    DtrEnable = false,
    RtsEnable = false
};

port.DataReceived += (_, _) =>
{
    while (port.BytesToRead > 0)
    {
        string line = port.ReadLine().Trim();
        DeviceEvent? evt = JsonSerializer.Deserialize<DeviceEvent>(line);
        if (evt?.type == "input")
        {
            // 推荐直接按 avatar 同步数字人，避免两端状态漂移。
            SwitchToAvatar(evt.avatar);
            Console.WriteLine($"seq={evt.seq}, ms={evt.ms}, {evt.source}/{evt.action}");
        }
    }
};

port.Open();
port.WriteLine("status");

record DeviceEvent(
    string type,
    uint seq,
    uint ms,
    string? source,
    string? action,
    int value,
    int avatar,
    string? name,
    string? event
);''',
        styles["CodeCN"],
    )
)

story.append(p("7. 联调注意事项", styles["H1CN"]))
story.append(
    table(
        [
            ["问题", "建议"],
            ["程序打不开 COM 口", "确认厂家烧录工具、监听脚本、PlatformIO Monitor 已关闭；串口同一时间只能被一个进程占用"],
            ["C# 收不到数据", "先发送 ping/status；确认端口、波特率 115200、ReadLine 换行符为 \\n"],
            ["延迟排查", "记录 C# 接收时刻，与消息中的 ms 比较；同时观察 seq 是否连续"],
            ["重复触发", "根据 seq 去重；根据 source/action/ms 设置业务侧防抖窗口"],
            ["人物不同步", "以设备消息里的 avatar 作为最终选择索引，C# 不要只根据 left/right 自己推算"],
            ["短按未触发", "目前现场已确认 long_press_start、knob left/right、touch click；button single_click 后续可单独调阈值"],
        ],
        [42 * mm, 124 * mm],
    )
)

story.append(p("8. 当前已验证结果", styles["H1CN"]))
story.append(
    Preformatted(
        '\n'.join(
            [
                "COM3 已识别为 Espressif USB 串口设备。",
                "芯片信息读取成功：ESP32-S3, Flash 16MB, PSRAM 8MB。",
                "设备端 JSON 串口协议已烧录并验证。",
                "PC -> Device: ping/status/next/prev 已验证。",
                "Device -> PC: knob left/right、touch click、button long_press_start 已抓取到事件。",
                "seq 与 ms 字段已加入并验证。",
            ]
        ),
        styles["CodeCN"],
    )
)

doc.build(story)
print(OUT)
