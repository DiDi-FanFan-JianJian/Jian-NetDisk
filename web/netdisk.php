<?php session_start();

    header('Content-type:text/html;charset=gbk');

    // �����û�е�¼������ת����¼ҳ��
    if (!isset($_SESSION['is_login']) || $_SESSION['is_login'] != 1) {
        // ��ʾ�û�δ��¼
        echo "<script>alert('���ȵ�¼��');</script>";
        echo "<script>window.location.href='index.php';</script>";
        exit();
    }

    // ����connect.php�ļ�
    require_once 'connect.php';

    // �����½��ļ�����uid�û���upath���½�filename���ļ�
    function handle_new_file($fname, $md5, $addfile)
    {
        $savepath = "/home/netdisk/files/";
        $dir_id = $_SESSION['dir_id'];

        if (($_FILES["file"]["error"] > 0 && $_FILES["file"]["error"] != 4) || (!$md5 && $_FILES["file"]["error"] > 0)) {
            switch ($_FILES["file"]["error"]) {
                case 1:
                    echo "<script>alert('�ϴ����ļ�������php.ini��upload_max_filesizeѡ�����Ƶ�ֵ');</script>";
                    break;
                case 2:
                    echo "<script>alert('�ϴ��ļ��Ĵ�С������HTML����MAX_FILE_SIZEѡ��ָ����ֵ');</script>";
                    break;
                case 3:
                    echo "<script>alert('�ļ�ֻ�в��ֱ��ϴ�');</script>";
                    break;
                case 4:
                    echo '<script>alert("��ѡ���ļ���");</script>';
                    break;
                case 6:
                    echo "<script>alert('�Ҳ�����ʱ�ļ���');</script>";
                    break;
                case 7:
                    echo "<script>alert('�ļ�д��ʧ��');</script>";
                    break;
            }
        } else {
            // �ж��Ƿ����ͬ���ļ�
            if (is_same_name($_SESSION['dir_id'], $fname)) {
                echo "<script>alert('�ļ����Ѵ��ڣ�');</script>";
            } else {
                // �����ļ���Ϣ
                if ($addfile != "miao") {
                    if (!move_uploaded_file($_FILES["file"]["tmp_name"], $savepath . $md5)) {
                        echo "<script>alert('�ļ��ƶ�ʧ�ܣ�');</script>";
                        return;
                    }
                }
                // �������ݿ���Ϣ
                if ($addfile == "miao") {
                    // �봫���ļ���С����Ҫ
                    if (insert_file($_SESSION['uid'], $md5, $dir_id, $fname, 0, $addfile)) {
                        echo "<script>alert('�ļ��봫�ɹ���');</script>";
                    } else {
                        echo "<script>alert('�ļ��봫ʧ�ܣ�');</script>";
                    }
                }
                else {
                    // �����봫����Ҫ�ļ���С
                    if (insert_file($_SESSION['uid'], $md5, $dir_id, $fname, $_FILES["file"]["size"], $addfile)) {
                        echo "<script>alert('�ļ��ϴ��ɹ���');</script>";
                    } else {
                        echo "<script>alert('�ļ��ϴ�ʧ�ܣ�');</script>";
                    }
                }
            }
        }
    }

    // ����ɾ���ļ�����uid�û���upath��ɾ��filename���ļ�
    function handle_delete_file($id)
    {
        $savepath = "/home/netdisk/files/";

        $dir_id = $_SESSION['dir_id'];
        $fid = get_fid_by_unique_id($id);
        $md5 = get_md5_by_fid($fid);
        if ($fid == -1) {
            echo "<script>alert('�ļ������ڣ�');</script>";
            return;
        }
        else {
            $count = get_file_count($fid);
            $is_last = 0;
            if ($count == 1) {
                // ɾ���ļ�
                unlink($savepath . $md5);
                $is_last = 1;
                if ($fid == $_SESSION['copyed_id']) {
                    $_SESSION['copyed_id'] = -1;
                    $_SESSION['copyed_name'] = "";
                    $_SESSION['is_clip'] = 0;
                }
            }
            // ɾ�����ݿ���Ϣ
            if (delete_file($id, $fid, $is_last)) {
                echo "<script>alert('�ļ�ɾ���ɹ���');</script>";
            } else {
                echo "<script>alert('�ļ�ɾ��ʧ�ܣ�');</script>";
            }
        }
    }

    // �ж�POST����
    if (isset($_POST['goto_dir'])) { // ��ת��ĳ��Ŀ¼
        // ��ȡĿ¼��
        $dir_name = $_POST['path'];
        $_SESSION['path'] = $dir_name;
        $_SESSION['dir_id'] = get_dir_id($_SESSION['uid'], $dir_name);
    }
    else if (isset($_POST['paste'])) { // ��copyed_idճ������ǰpath�£����ƻ��߼��У�
        // ��ȡճ�����ļ�id
        $copyed_id = $_SESSION['copyed_id'];
        $copyed_name = $_POST['paste_fname'];
        if (is_same_name($_SESSION['dir_id'], $copyed_name)) {
            echo "<script>alert('�ļ����Ѵ��ڣ�');</script>";
        }
        else {
        // ����Ǽ���
            if ($_SESSION['is_clip']) {
                if (paste_file_by_clip($copyed_id, $copyed_name, $_SESSION['dir_id'])) {
                    echo "<script>alert('�ļ����гɹ���');</script>";
                    $_SESSION['is_clip'] = 0;
                    $_SESSION['copyed_id'] = -1;
                    $_SESSION['copyed_name'] = "";
                } else {
                    echo "<script>alert('�ļ�����ʧ�ܣ�');</script>";
                }
            }
            else {
                if (paste_file_by_copy($_SESSION['uid'], $copyed_id, $copyed_name, $_SESSION['dir_id'])) {
                    echo "<script>alert('�ļ����Ƴɹ���');</script>";
                } else {
                    echo "<script>alert('�ļ�����ʧ�ܣ�');</script>";
                }
            }
        }
    }
    else if (isset($_POST['rename'])) { // ��������unique_id��
        // ��ȡunique_id
        $unique_id = $_POST['id'];
        // ��ȡ������
        $new_name = $_POST['new_fname'];

        if (is_same_name($_SESSION['dir_id'], $new_name)) {
            echo "<script>alert('�ļ����Ѵ��ڣ�');</script>";
        } else {
            if (rename_file($unique_id, $new_name)) {
                echo "<script>alert('�������ɹ���');</script>";
            } else {
                echo "<script>alert('������ʧ�ܣ�');</script>";
            }
        }
    }
    else if (isset($_POST['copyed'])) { // ���ƣ�fid��
        // ��ȡfid
        $fid = $_POST['copyed_id'];
        // ��ȡ����
        $fname = $_POST['copyed_name'];
        // ���ø���״̬
        $_SESSION['is_clip'] = 0;
        // ���ø��Ƶ��ļ�id
        $_SESSION['copyed_id'] = $fid;
        // ���ø��Ƶ��ļ���
        $_SESSION['copyed_name'] = $fname;

        echo "<script>alert('�����ļ���$fname');</script>";
    }
    else if (isset($_POST['clip'])) { // ���У�unique_id��
        // ��ȡunique_id
        $unique_id = $_POST['clip_id'];
        // ��ȡ����
        $fname = $_POST['clip_name'];
        // ���ü���״̬
        $_SESSION['is_clip'] = 1;
        // ���ü��е��ļ�id
        $_SESSION['copyed_id'] = $unique_id;
        // ���ü��е��ļ���
        $_SESSION['copyed_name'] = $fname;

        echo "<script>alert('�����ļ���$fname');</script>";
    }
    else if (isset($_POST['delfile'])) { // ɾ����unique_id��
        // ��ȡunique_id
        $unique_id = $_POST['delfile_id'];
        if ($unique_id == $_SESSION['copyed_id'] && $_SESSION['is_clip']) {
            $_SESSION['copyed_id'] = -1;
            $_SESSION['copyed_name'] = "";
            $_SESSION['is_clip'] = 0;
        }
        handle_delete_file($unique_id);
    }
    else if (isset($_POST['add_dir_name'])) { // ������Ŀ¼
        // ��ȡĿ¼��
        $dir_name = $_POST['add_dir_name'];
        if (is_same_name($_SESSION['dir_id'], $dir_name)) {
            echo "<script>alert('Ŀ¼���Ѵ��ڣ�');</script>";
        }
        else {
            // ����Ŀ¼
            $tem_id = get_next_id("dirs");
            insert_dir($tem_id, $_SESSION['uid'], $_SESSION['dir_id'], $dir_name);
            echo "<script>alert('����Ŀ¼��${dir_name}�ɹ�');</script>";
        }
    }
    else if (isset($_POST['addfile'])) { // �������ļ�
        // ��ȡ�ļ���
        $file_name = $_POST['fname'];
        // ��ȡmd5ֵ
        $md5 = $_POST['md5'];
        // ��ȡaddfile
        $addfile = $_POST['addfile'];

        handle_new_file($file_name, $md5, $addfile);
    }

    // �����й�ʱ��
    date_default_timezone_set("PRC");

    // ��ȡ�ļ��б�
    $dir_list = get_dir_list($_SESSION['dir_id']);
    $file_list = get_file_list($_SESSION['dir_id']);
    // ����netdisk.html�ļ�
    require_once './view/netdisk.html';
    
    //�ر����ݿ�
    $mysql->close();
?>