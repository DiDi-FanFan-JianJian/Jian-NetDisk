<?php session_start();
    /*
     * ��¼ע��ҳ��
     * �����get�����򷵻�index.htmlҳ��
     * �����post�������ж�������
     *    �����¼�ɹ����򵯿���ʾ������ת��index.phpҳ��
     *    �����¼ʧ�ܣ��򵯿���ʾ������Ϣ��������index.phpҳ��
     * 
     *    ���ע��ɹ����򵯿���ʾע��ɹ���������index.phpҳ��
     *    ���ע��ʧ�ܣ��򵯿���ʾ������Ϣ��������index.phpҳ��
     */

    header("Content-Type: text/html; charset=gbk");
    $_SESSION['username'] = "";
    $_SESSION['password'] = "";
    $_SESSION['uid'] = "";
    $_SESSION['path'] = "";
    $_SESSION['dir_id'] = -1;
    $_SESSION['copyed_id'] = -1;
    $_SESSION['copyed_name'] = '';
    $_SESSION['is_clip'] = 0;
    $_SESSION['is_login'] = 0;
    // �ж���������
    if ($_SERVER['REQUEST_METHOD'] == 'GET') {
        include './view/index.html';
    } else if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        $username = $_POST['username'];
        $password = $_POST['password'];

        if ($username && $password) { // ����û��������붼��Ϊ��
            // �������ݿ�����
            include './connect.php';

            if (isset($_POST["signup"])) { // �ж�ע��
                if (is_user_exist($username)) { // �û����Ѵ���
                    echo "<script>alert('�û����Ѵ��ڣ�3�����ת�ص�¼/ע��ҳ��');</script>";
                    echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                } else { // �û���������
                    $r1 = '/[A-Z]/';  //uppercase
                    $r2 = '/[a-z]/';  //lowercase
                    $r3 = '/[0-9]/';  //numbers
                    $r4 = '/[~!@#$%^&*()\-_=+{};:<,.>?]/';  // special char
                    $flag = 0;
                    if (preg_match($r1, $password)) {
                        $flag++;
                    }
                    if (preg_match($r2, $password)) {
                        $flag++;
                    }
                    if (preg_match($r3, $password)) {
                        $flag++;
                    }
                    if (preg_match($r4, $password)) {
                        $flag++;
                    }
                    if ($flag < 3 || strlen($password) < 12) {
                        echo "<script>alert('������������д��ĸ��Сд��ĸ�����֡������ַ��е����֣��ҳ��Ȳ�С��12��3�����ת�ص�¼/ע��ҳ��');</script>";
                        echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                    } else {
                        // ���ݿ�������û�����Ŀ¼
                        $user_id = get_next_id("users");
                        $root_id = get_next_id('dirs');
                        insert_user($user_id, $username, $password, $root_id);
                        insert_dir($root_id, $user_id, 0, 'root');
    
                        echo "<script>alert('ע��ɹ���3�����ת�ص�¼/ע��ҳ��');</script>";
                        echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                    }
                }
            } else { // �жϵ�¼
                if (!is_user_exist($username)) { // �û���������
                    echo "<script>alert('�û��������ڣ�3�����ת�ص�¼/ע��ҳ��');</script>";
                    echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                }
                else if (check_password($username, $password)) {// ��¼�ɹ�
                    $uid = get_user_id($username);
                    $_SESSION['username'] = $username;
                    $_SESSION['password'] = $password;
                    $_SESSION['uid'] = $uid;
                    $_SESSION['path'] = 'root/';
                    $_SESSION['dir_id'] = get_dir_id($uid, 'root/');
                    $_SESSION['copyed_id'] = -1;
                    $_SESSION['copyed_name'] = '';
                    $_SESSION['is_clip'] = 0;
                    $_SESSION['is_login'] = 1;
                    // ��ת��netdisk.phpҳ��
                    echo "<script>alert('��¼�ɹ���3�����ת������ҳ��');</script>";
                    header('Location: netdisk.php');
                } else { // ��¼ʧ��
                    echo "<script>alert('�������3�����ת�ص�¼/ע��ҳ��');</script>";
                    echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                }
            }

            // �ر����ݿ�����
            $mysql->close();
        } else { // ����û���������Ϊ��
            echo "����д��������3�����ת�ص�¼/ע��ҳ��";
            echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
        }
    }
?>
