<?php session_start();
$file_name  = $_POST['filename'];
$savepath = "/home/netdisk/files/";
$file_path  = $savepath . $_POST['md5'];

set_time_limit(0);
// ��Ҫ���ص��ļ�
$file = @fopen($file_path,"rb");
// while (ob_get_level()) ob_end_clean();
// �����ļ���ǩ
header('Content-Description: File Transfer');
header('Content-Type: application/octet-stream');
header('Content-Disposition: attachment; filename='.$_POST['filename']);
header('Content-Transfer-Encoding: binary');
header('Expires: 0');
header('Cache-Control: must-revalidate');
header('Pragma: public');
header('Content-Length: ' . filesize($file_path));
// ����ļ�����
while (!feof($file)) {
    print(@fread($file, 8 * 1024));
    ob_flush();
    flush();
}
