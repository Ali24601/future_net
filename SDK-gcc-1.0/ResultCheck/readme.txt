result检测
ResultCheck.sh文件说明：
./ResultCheck ../casehard/case1
第一个参数为同目录二进制文件
第二个参数为测试文件夹目录，修改可以测试不同case

针对case给出的解，判断解的有效性。
输出结果说明（优先度依次降低）：
1.Repass link.
2.Repass node.
3.First node wrong.
4.Path break.
5.Link unexist.
6.Unreached node.
7.Last node wrong.
8.Right Answer.

只判断解的有效性，对于无解（NA）的情况不能正确判断。

判题错误信息详解：http://bbs.csdn.net/topics/391919833
