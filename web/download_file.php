<?php session_start();
$path_parts = pathinfo($_POST['md5']);
$file_name  = $path_parts['basename'];
$savepath = "D:\\tem\\";
$file_path  = $savepath . $_POST['md5'];
set_time_limit(0);
$file = @fopen($file_path,"rb");
// echo $file_path;
// while (ob_get_level()) ob_end_clean();
header('Content-Description: File Transfer'); //描述页面返回的结果
header('Content-Type: application/octet-stream'); //返回内容的类型，此处只知道是二进制流。具体返回类型可参考http://tool.oschina.net/commons
header('Content-Disposition: attachment; filename='.$_POST['fname']);//可以让浏览器弹出下载窗口
header('Content-Transfer-Encoding: binary');//内容编码方式，直接二进制，不要gzip压缩
header('Expires: 0');//过期时间
header('Cache-Control: must-revalidate');//缓存策略，强制页面不缓存，作用与no-cache相同，但更严格，强制意味更明显
header('Pragma: public');
header('Content-Length: ' . filesize($file_path)); //文件大小，在文件超过2G的时候，filesize()返回的结果可能不正确
while (!feof($file)) {
    print(@fread($file, 8 * 1024));
    ob_flush();
    flush();
}
// //设置完header以后
// ob_clean();
// flush();  //清空缓冲区
// readfile($file_path);