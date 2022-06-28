<?php session_start();
header('Content-type:text/html;charset=gbk');
$md5 = $_POST["md5"];
//打开数据库
$mysql = new mysqli('1.15.144.212', 'root', 'root123', 'netdisk');
if (mysqli_connect_errno()) {
    echo "连接失败，原因为：" . mysqli_connect_error();
    exit();
}
//设置中文字符集
$result = $mysql->query("set names gbk");

$sql = "select * from files where md5 = '$md5'"; 
$result = $mysql->query($sql); //执行sql
$rows = $result->num_rows;
if ($rows) {
    echo 1;
} else {
    echo 0;
}
