<?php session_start();
header('Content-type:text/html;charset=gbk');
$md5 = $_POST["md5"];
//�����ݿ�
$mysql = new mysqli('localhost', 'root', 'root123', 'netdisk');
if (mysqli_connect_errno()) {
    echo "<script>alert('���ݿ�����ʧ�ܣ�');</script>";
    exit();
}
//���������ַ���
$result = $mysql->query("set names gbk");

$sql = "select * from files where md5 = '$md5'"; 
$result = $mysql->query($sql); //ִ��sql
$rows = $result->num_rows;
if ($rows) {
    echo 1;
} else {
    echo 0;
}
