## x86 machine code 初探 (0) - base register address mode (作者：陳鍾誠)

### 組譯範例

組合語言：sum.as0

```
        LD     R1, sum      ; R1 = sum = 0
        LD     R2, i        ; R2 = i = 1
        LDI    R3, 10       ; R3 = 10
FOR:    CMP    R2, R3       ; if (R2 > R3)
        JGT    EXIT         ;   goto EXIT
        ADD    R1, R1, R2   ; R1 = R1 + R2 (sum = sum + i)
        ADDI   R2, R2, 1    ; R2 = R2 + 1  ( i  = i + 1)
        JMP    FOR          ; goto FOR
EXIT:   ST     R1, sum      ; sum = R1
        ST     R2, i        ; i = R2
        LD     R9, msgptr   ; R9= pointer(msg) = &msg
        SWI    3            ; SWI 3 : print string &msg
        MOV    R9, R1       ; R9 = R1 = sum
        SWI    2            ; SWI 2 : print number sum
        RET                 ; return to CALLER
i:      RESW    1           ; int i
sum:    WORD    0           ; int sum=0
msg:    BYTE    "sum=", 0   ; char *msg = "sum="
msgptr: WORD    msg         ; char &msgptr = &msg
```

組譯過程

```
D:\Dropbox\Public\oc\code>node as0 sum.as0 sum.ob0
Assembler:asmFile=sum.as0 objFile=sum.ob0
===============Assemble=============
[ '        LD     R1, sum      ; R1 = sum = 0',
  '        LD     R2, i        ; R2 = i = 1',
  '        LDI    R3, 10       ; R3 = 10',
  'FOR:    CMP    R2, R3       ; if (R2 > R3)',
  '        JGT    EXIT         ;   goto EXIT',
  '        ADD    R1, R1, R2   ; R1 = R1 + R2 (sum = sum + i)',
  '        ADDI   R2, R2, 1    ; R2 = R2 + 1  ( i  = i + 1)',
  '        JMP    FOR          ; goto FOR',
  'EXIT:   ST     R1, sum      ; sum = R1',
  '        ST     R2, i        ; i = R2',
  '        LD     R9, msgptr   ; R9= pointer(msg) = &msg',
  '        SWI    3            ; SWI 3 : print string &msg',
  '        MOV    R9, R1       ; R9 = R1 = sum',
  '        SWI    2            ; SWI 2 : print number sum',
  '        RET                 ; return to CALLER',
  'i:      RESW    1           ; int i',
  'sum:    WORD    0           ; int sum=0',
  'msg:    BYTE    "sum=", 0   ; char *msg = "sum="',
  'msgptr: WORD    msg         ; char &msgptr = &msg' ]
=================PASS1================
0000          LD       R1,sum           L 00
0004          LD       R2,i             L 00
0008          LDI      R3,10            L 08
000C FOR      CMP      R2,R3            A 10
0010          JGT      EXIT             J 23
0014          ADD      R1,R1,R2         A 13
0018          ADDI     R2,R2,1          A 1B
001C          JMP      FOR              J 26
0020 EXIT     ST       R1,sum           L 01
0024          ST       R2,i             L 01
0028          LD       R9,msgptr        L 00
002C          SWI      3                J 2A
0030          MOV      R9,R1            A 12
0034          SWI      2                J 2A
0038          RET                       J 2C
003C i        RESW     1                D F0
0040 sum      WORD     0                D F2
0044 msg      BYTE     "sum=",0         D F3
0049 msgptr   WORD     msg              D F2
===============SYMBOL TABLE=========
FOR      000C
EXIT     0020
i        003C
sum      0040
msg      0044
msgptr   0049
=============PASS2==============
0000          LD       R1,sum           L 00 001F003C
0004          LD       R2,i             L 00 002F0034
0008          LDI      R3,10            L 08 0830000A
000C FOR      CMP      R2,R3            A 10 10230000
0010          JGT      EXIT             J 23 2300000C
0014          ADD      R1,R1,R2         A 13 13112000
0018          ADDI     R2,R2,1          A 1B 1B220001
001C          JMP      FOR              J 26 26FFFFEC
0020 EXIT     ST       R1,sum           L 01 011F001C
0024          ST       R2,i             L 01 012F0014
0028          LD       R9,msgptr        L 00 009F001D
002C          SWI      3                J 2A 2A000003
0030          MOV      R9,R1            A 12 12910000
0034          SWI      2                J 2A 2A000002
0038          RET                       J 2C 2C000000
003C i        RESW     1                D F0 00000000
0040 sum      WORD     0                D F2 00000000
0044 msg      BYTE     "sum=",0         D F3 73756D3D00
0049 msgptr   WORD     msg              D F2 00000044
=================SAVE OBJ FILE================

00 :  001F003C 002F0034 0830000A 10230000
10 :  2300000C 13112000 1B220001 26FFFFEC
20 :  011F001C 012F0014 009F001D 2A000003
30 :  12910000 2A000002 2C000000 00000000
40 :  00000000 73756D3D 00000000 44
```

### AS0 組譯器設計

檔案：as.js

```javascript
var fs = require("fs");
var c = require("./ccc");
var Memory = require("./memory");

var as = function(opTable) {
 this.opTable = opTable; 

 this.assemble = function(asmFile, objFile) { // 組譯器的主要函數
  this.lines = []; this.symTable = {}; this.codes = [];
  c.log("Assembler:asmFile=%s objFile=%s", asmFile, objFile); // 輸入組合語言、輸出目的檔
  c.log("===============Assemble=============");
  var text = fs.readFileSync(asmFile, "utf8");                // 讀取檔案到 text 字串中
  this.lines = text.split(/[\r\n]+/);                              // 將組合語言分割成一行一行
  c.log(this.lines);
  this.pass1();                                       // 第一階段：計算位址
  c.log("===============SYMBOL TABLE=========");              // 印出符號表
  for (s in this.symTable) {
    c.log("%s %s", c.fill(' ',s,8), c.hex(this.symTable[s].address, 4));
  }
  this.pass2();     // 第二階段：建構目的碼
  this.saveObjFile(objFile);    // 輸出目的檔
 }

 this.pass1 = function() {                // 第一階段的組譯
  var address = 0;
  c.log("=================PASS1================");
  for (var i in this.lines) {                                      // 對於每一行
    try {
      var code = this.parse(this.lines[i]);              // 剖析並建立 code 物件
      code.address = address;                             // 設定該行的位址
      if (code.label.length != 0) {                        // 如果有標記符號
        this.symTable[code.label] = code;                 //    加入符號表中
      }
      this.codes.push(code);
      c.log("%s", code);
      address += this.size(code);                             //  計算下一個指令位址
    } catch (err) {
      c.error(c.format("line %d : %s", i, this.lines[i]), err);
    }
  }
 }

 this.pass2 = function(codes) {                       // 組譯器的第二階段
  c.log("=============PASS2==============");
  for (var i in this.codes) {                        // 對每一個指令
    try {
      this.translate(this.codes[i]);
      c.log("%s", this.codes[i]);
    } catch (err) {
      c.error(c.format("line %d : %s", i, this.lines[i]), err);
    }
  }
 }
 
 this.saveObjFile = function(objFile) {
  c.log("=================SAVE OBJ FILE================");
  var obj = "";
  for (var i in this.codes)
    obj += this.codes[i].obj;
  var m = new Memory(1);
  m.loadhex(obj);
  m.dump();
  m.save(objFile);
 }

 this.size = function(code) {
    var len = 0, unitSize = 1;
    switch (code.op.name) {                             // 根據運算碼 op
      case "RESW" : return 4 * parseInt(code.args[0]);  // 如果是 RESW, 大小為 4*保留量(參數 0)
      case "RESB" : return 1 * parseInt(code.args[0]);  // 如果是 RESB, 大小為 1*保留量(參數 0)
      case "WORD" : 
        unitSize = 4;
      case "BYTE" : 
        for (i in code.args) {
          if (code.args[i].match(/^\".*?\"$/))
            len += (code.args[i].length - 2) * unitSize;
          else
            len += unitSize;
        }
        return len;        // 如果是BYTE, 大小是 1*參數個數
      case "" :     return 0;                           // 如果只是標記, 大小為 0
      default :     return 4;                           // 其他情形 (指令), 大小為 4
    }
 }  
}

module.exports = as;
```

檔案：as0.js

```javascript
var c = require("./ccc");
var as = require("./as");
var code = require("./code");
var cpu0 = require("./cpu0");

var as0 = new as(cpu0.opTable);

as0.parse = function(line) {
    return new code(line, this.opTable);
}

as0.translate = function(code) { // 指令的編碼函數
  var ra=0, rb=0, rc=0, cx=0;
  var pc = code.address + 4;                     // 提取後PC為位址+4
  var args = code.args, parseR = code.parseR;
  var labelCode = null;
  switch (code.op.type) {                        // 根據指令型態
    case 'J' :                                   // 處理 J 型指令
      switch (code.op.name) {
        case "RET": case "IRET" : 
            break;
        case "SWI" : 
            cx = parseInt(args[0]);
            break;
        default :
            labelCode = this.symTable[args[0]];           //   取得符號位址
            cx = labelCode.address - pc;             //   計算 cx 欄位
            break;
      }
      code.obj = c.hex(code.op.id,2)+c.hex(cx, 6); //   編出目的碼(16進位)
      break;
    case 'L' :
      ra = parseR(args[0]);
      switch (code.op.name) {
        case "LDI" : 
            cx = parseInt(args[1]);
            break;
        case "LDR": case "LBR": case "STR": case "SBR":
            rb = parseR(args[1]);
            rc = parseR(args[2]);
            break;
        default :
            if (args[1].match(/^[a-zA-Z]/)){ // LABEL
              labelCode = this.symTable[args[1]];
              rb = 15; // R[15] is PC
              cx = labelCode.address - pc;
            } else {
              rb = 0;
              cx = parseInt(args[2]);
            }
            break;
      }
      code.obj = c.hex(code.op.id, 2)+c.hex(ra, 1)+c.hex(rb, 1)+c.hex(cx, 4);
      break;
    case 'A' :                                   // 處理 A 型指令
      ra = parseR(args[0]);
      switch (code.op.name) {
        case "CMP": case "MOV" : 
            rb = parseR(args[1]);
            break;
        case "SHL": case "SHR": case "ADDI":
            rb = parseR(args[1]);
            cx = parseInt(args[2]);
            break;
        case "PUSH": case "POP": case "PUSHB": case "POPB" : 
            break;
        default : 
            rb = parseR(args[1]);
            rc = parseR(args[2]); 
            break;
      }
      code.obj = c.hex(code.op.id, 2)+c.hex(ra, 1)+c.hex(rb, 1)+c.hex(rc,1)+c.hex(cx, 3);
      break;
    case 'D' : { // 我們將資料宣告  RESW, RESB, WORD, BYTE 也視為一種指令，其形態為 D
      var unitSize = 1;
      switch (code.op.name) {                    
        case "RESW":  case "RESB": // 如果是 RESW 或 RESB
          code.obj = c.dup('0', this.size(code)*2);
          break;                            
        case "WORD":                             // 如果是 WORD ，佔 4 個 byte
          unitSize = 4;
        case "BYTE": {                           // 如果是 BYTE ，佔 1 個 byte
          code.obj = "";
          for (var i in args) {
            if (args[i].match(/^\".*?\"$/)) {  // 字串，例如： "Hello!" 轉為 68656C6C6F21
              var str = args[i].substring(1, args[i].length-1);
              code.obj += c.str2hex(str);
            } else if (args[i].match(/^\d+$/)) {  // 常數
              code.obj += c.hex(parseInt(args[i]), unitSize*2);
            } else {                               // 標記
              labelCode = this.symTable[args[i]];
              code.obj += c.hex(labelCode.address, unitSize*2);
            }
          }
          break;
        } // case BYTE:
      } // switch
      break;
    } // case 'D'
  }
} 

as0.assemble(process.argv[2], process.argv[3]);
```
