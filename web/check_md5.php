<?php session_start();
header('Content-type:text/html;charset=gbk');
$md5 = $_POST["md5"];
//打开数据库
$mysql = new mysqli('localhost', 'root', 'root123', 'netdisk');
if (mysqli_connect_errno()) {
    echo "<script>alert('数据库连接失败！');</script>";
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
