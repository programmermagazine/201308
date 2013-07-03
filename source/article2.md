## JavaScript (8) – 正規表達式的運用 (作者：陳鍾誠)



## 正規表達式程式範例

程式範例：regexp.js

```javascript
var re = new RegExp("\\d+", "gi");
var str = "name:john age:20 birthday:1990/8/31";

var m = null;
while (m = re.exec(str))
    console.log(m.toString());
    
var p = parse(str);
console.log("p.name="+p.name+" age="+p.age+" year="+p.year+" month="+p.month+" day="+p.day);

function parse(data) {
    var e=new RegExp("name:(\\w+) age:(\\d+) birthday:(\\d+)/(\\d+)/(\\d+)", "gi");

    if (data.match(e)) {
        return  {exp: RegExp['$&'],
                name: RegExp.$1,
                age:RegExp.$2,
                year:RegExp.$3,
                month:RegExp.$4,
                day:RegExp.$5};
    }
    else {
        return null;
    }
}

String.prototype.trim = function() { 
  return this.replace(/(^\s*)|(\s*$)/g, ""); 
}

console.log("' abc '.trim()='"+' abc '.trim()+"'");
```

執行結果

```
D:\code\node>node regexp.js
' abc '.trim()='abc'
20
1990
8
31
p.name=john age=20 year=1990 month=8 day=31

D:\code\node>node RegexpTest.js
20
1990
8
31
p.name=john age=20 year=1990 month=8 day=31
' abc '.trim()='abc'
```

### 程式範例：將內嵌於 Markdown 中的 Tex 數學式轉換為影像檔連結

程式：md0.js

```javascript
var http = require('http');
var fs = require("fs");
var crypto = require('crypto');
var child_process = require('child_process');
var log = console.log;
var argv = process.argv;

var md0 = fs.readFileSync(argv[2], "utf8");
var md  = tex2md(md0);
fs.writeFileSync(argv[3], md, "utf8");

function hash(o) {
  var hash = crypto.createHash('md5');
  hash.update(o.toString());
  return hash.digest('hex');
}

function tex2md(md0) {
  var md  = "";
  re = new RegExp(/\$(\S.*?\S)\$/gm); // *?, +? non greedy, m for multiline
  var lastEnd = 0;
//  var imgUrl = "http://chart.apis.google.com/chart?cht=tx&amp;chl=[etex]";
  var texPattern = " ![]([imgpath].jpg) ";
  while((m = re.exec(md0)) !== null) {
    var tex = m[1];
    var imgpath = "../timg/"+tex.replace(/\W+/g, "_").substr(0,8)+"_"+hash(tex);
    tex2file(tex, imgpath);
    md += md0.substring(lastEnd, m.index)+texPattern.replace("[tex]", tex).replace("[imgpath]", imgpath);
    lastEnd = m.index + m[0].length;
  }
  md += md0.substring(lastEnd);
  return md;
}

/* Mimetex 的下載、編譯及用法請參考：http://www.forkosh.com/mimetexmanual.html
執行範例：
D:\201305\mimetex>gcc -DAA mimetex.c gifsave.c -lm -o mimetex.cgi
D:\201305\mimetex>gcc -DAA mimetex.c gifsave.c -lm -o mimetex
D:\201305\mimetex>mimetex -d "x^2+y^2" > exp.gif

影像從 gif 轉為 jpg 的方法，使用 imagemagic (command line)
參考：http://www.imagemagick.org/script/convert.php
例如：convert rose.gif rose.jpg */

function tex2file(tex, filename) {
  var command = 'tex2img.bat "'+tex+'" '+filename;
  child_process.exec(command, function(err, stdout, stderr) {
      log(command+" finished!\n");
      log("err="+err);
  });
}
```

### 參考

