<?php session_start();
$path_parts = pathinfo($_POST['md5']);
$file_name  = $path_parts['basename'];
$savepath = "D:\\tem\\";
$file_path  = $savepath . $_POST['md5'];
set_time_limit(0);
$file = @fopen($file_path,"rb");
// echo $file_path;
// while (ob_get_level()) ob_end_clean();
header('Content-Description: File Transfer'); //����ҳ�淵�صĽ��
header('Content-Type: application/octet-stream'); //�������ݵ����ͣ��˴�ֻ֪���Ƕ������������巵�����Ϳɲο�http://tool.oschina.net/commons
header('Content-Disposition: attachment; filename='.$_POST['fname']);//������������������ش���
header('Content-Transfer-Encoding: binary');//���ݱ��뷽ʽ��ֱ�Ӷ����ƣ���Ҫgzipѹ��
header('Expires: 0');//����ʱ��
header('Cache-Control: must-revalidate');//������ԣ�ǿ��ҳ�治���棬������no-cache��ͬ�������ϸ�ǿ����ζ������
header('Pragma: public');
header('Content-Length: ' . filesize($file_path)); //�ļ���С�����ļ�����2G��ʱ��filesize()���صĽ�����ܲ���ȷ
while (!feof($file)) {
    print(@fread($file, 8 * 1024));
    ob_flush();
    flush();
}
// //������header�Ժ�
// ob_clean();
// flush();  //��ջ�����
// readfile($file_path);