<?php
// ���ݿ���Ϣ
$db_host = "localhost"; //����
$db_username = "root"; //���ݿ��û���
$db_password = "root123"; //���ݿ�����
$db_name = "netdisk";
$mysql = new mysqli($db_host, $db_username, $db_password, $db_name);
if ($mysql->connect_error) {
    // �������ʧ��������󣬲��˳�
    die("can't connect" . $mysql->connection_error);
    exit();
} else {
    $result = $mysql->query("set names gbk");
}

// �ж���Ϣ
// �ж��û��Ƿ����
function is_user_exist($username)
{
    global $mysql;
    $sql = "select * from users where username = '$username'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        return true;
    } else {
        return false;
    }
}
// �ж��ļ����Ƿ��Ѿ����ڣ�uid�û���pid�ļ������Ƿ���dir_name
function is_dir_exist($pid, $dir_name)
{
    global $mysql;
    $sql = "select * from dirs where pid = '$pid' and name = '$dir_name'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        return true;
    } else {
        return false;
    }
}
// �ж��ļ��Ƿ��Ѿ����ڣ�did�ļ������Ƿ���filename
function is_file_exist($did, $filename)
{
    global $mysql;
    $sql = "select * from file_dir where did = '$did' and filename = '$filename'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        return true;
    } else {
        return false;
    }
}
// �ж��Ƿ���ͬ��
function is_same_name($dir_id, $name)
{
    if (!is_dir_exist($dir_id, $name) && !is_file_exist($dir_id, $name)) {
        return false;
    } else {
        return true;
    }
}
// �ж��û������Ƿ���ȷ
function check_password($username, $password)
{
    global $mysql;
    $sql = "select * from users where username = '$username' and password='" . md5($password) . "'";
    // $sql = "select * from users where username = '$username' and password = '$password'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        return true;
    } else {
        return false;
    }
}

// ��ȡ��Ϣ
// ��ȡ��һ��id
function get_next_id($table) {
    global $mysql;
    $sql = "select max(id) from $table";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        $row = $result->fetch_row();
        return $row[0] + 1;
    } else {
        return 1;
    }
}
// ��ȡָ���û���id���û�id��1��ʼ������-1��ʾ�����ڣ�
function get_user_id($username)
{
    global $mysql;
    $sql = "select * from users where username = '$username'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        $row = $result->fetch_assoc();
        return $row["id"];
    } else {
        return -1;
    }
}
// ����path��ȡ��ǰĿ¼id���ļ�id��1��ʼ������-1��ʾ�����ڣ�
function get_dir_id($uid, $path)
{
    global $mysql;
    $path = trim($path, "/");
    $path = explode("/", $path);
    $dir_id = 0;
    // ÿ���û���Ŀ¼��pidΪ0�����û���Ŀ¼��ʼ��ѯ
    $pid = 0;
    foreach ($path as $dir) {
        if ($dir == "") {
            continue;
        }
        $sql = "select * from dirs where name = '$dir' and pid = $pid and uid = $uid";
        $result = $mysql->query($sql);
        $rows = $result->num_rows;
        if ($rows) {
            $row = $result->fetch_assoc();
            $dir_id = $row['id'];
            $pid = $dir_id;
        } else {
            return -1;
        }
    }
    return $dir_id;
}
// ��ȡָ��md5���ļ�id���ļ�id��1��ʼ������-1��ʾ�����ڣ�
function get_file_id_by_md5($md5)
{
    global $mysql;
    $sql = "select id from files where md5 = '$md5'";
    $result = $mysql->query($sql);
    // �����ѯ���Ϊ�գ��򷵻�-1
    if ($result->num_rows == 0) {
        return -1;
    }
    // �����ѯ�����Ϊ�գ��򷵻��ļ�id
    else {
        $row = $result->fetch_assoc();
        return $row['id'];
    }
}
// ��ȡָ��fid��md5
function get_md5_by_fid($fid)
{
    global $mysql;
    $sql = "select md5 from files where id = '$fid'";
    $result = $mysql->query($sql);
    $row = $result->fetch_assoc();
    return $row['md5'];
}
// ����did��filename��ȡ�ļ�id���ļ�id��1��ʼ������-1��ʾ�����ڣ�
function get_file_id($did, $filename)
{
    global $mysql;
    $sql = "select * from file_dir where did = '$did' and filename = '$filename'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        $row = $result->fetch_assoc();
        return $row['id'];
    } else {
        return -1;
    }
}
// ��ȡunique_id��Ӧ��fid
function get_fid_by_unique_id($unique_id)
{
    global $mysql;
    $sql = "select * from file_dir where id = '$unique_id'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        $row = $result->fetch_assoc();
        return $row['fid'];
    } else {
        return -1;
    }
}
// ��ȡfid��Ӧ�ļ���count
function get_file_count($fid)
{
    global $mysql;
    $sql = "select * from files where id = '$fid'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    if ($rows) {
        $row = $result->fetch_assoc();
        return $row['count'];
    } else {
        return -1;
    }
}
// ��ȡָ���û����ļ����б��������飬ÿ��Ԫ����һ���ļ��е���Ϣ��
function get_dir_list($dir_id)
{
    global $mysql;
    $sql = "select * from dirs where pid = '$dir_id'";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    $dir_list = array();
    if ($rows) {
        while ($row = $result->fetch_assoc()) {
            $dir_list[] = $row;
        }
    }
    return $dir_list;
}
// ��ȡָ���û����ļ��б��������飬ÿ��Ԫ����һ���ļ�����Ϣ��
function get_file_list($dir_id)
{
    global $mysql;
    $sql = "select file_dir.id as unique_id, files.id as fid, size, md5, filename from file_dir, files where file_dir.did = '$dir_id' and file_dir.fid = files.id";
    $result = $mysql->query($sql);
    $rows = $result->num_rows;
    $file_list = array();
    if ($rows) {
        while ($row = $result->fetch_assoc()) {
            $file_list[] = $row;
        }
    }
    return $file_list;
}


// users        (id, username, password, root����Ŀ¼�����֣�);
// files        (id, size, md5, path���ļ�ʵ�ʴ洢λ�ã�, count);

// dirs         (id, uid, pid, name���ļ������֣�, create_time, update_time);
// file_dir    (id, fid, did, uid, filename���ļ�����, create_time, update_time);


// ��������
// �������û����������Ϊmd5ֵ��
function insert_user($id, $username, $password, $root)
{
    global $mysql;
    $sql = "insert into users (id, username, password, root) values ('$id', '$username', '" . md5($password) . "', '$root')";
    // $sql = "insert into users (id, username, password, root) values ('$id', '$username',  '$password', '$root')";
    $result = $mysql->query($sql);
    if ($result) {
        return true;
    } else {
        return false;
    }
}
// �����ļ���
function insert_dir($id, $uid, $pid, $name)
{
    global $mysql;
    $sql = "insert into dirs (id, uid, pid, name, create_time, update_time) values ($id, '$uid', '$pid', '$name', now(), now())";
    $result = $mysql->query($sql);
    if ($result) {
        return true;
    } else {
        return false;
    }
}
// �����ļ�
function insert_file($uid, $md5, $dir_id, $fname, $size, $addfile)
{
    $savepath = "/home/netdisk/files/";
    global $mysql;
    if ($addfile != 'miao') {
        // û�и��ļ��������ļ�
        $fid = get_next_id('files');
        $sql = "insert into files(id, size, md5, path, count) values ('$fid', '$size', '$md5', '" . $savepath . $md5 . "', '1')";
        $mysql->query($sql);

        // �����ļ����ļ�����
        $tem_id = get_next_id('file_dir');
        $sql = "insert into file_dir(uid, id, fid, did, filename, create_time, update_time) values ('$uid', '$tem_id', '$fid', '$dir_id', '$fname', now(), now())";
        $result = $mysql->query($sql);
        if ($result) {
            return true;
        } else {
            return false;
        }
    }
    else {
        // �и��ļ��������ļ���Ϣ
        $fid = get_file_id_by_md5($md5);
        $sql = "update files set count = count + 1 where id = '$fid'";
        $mysql->query($sql);

        // �����ļ����ļ�����
        $tem_id = get_next_id('file_dir');
        $sql = "insert into file_dir(uid, id, fid, did, filename, create_time, update_time) values ('$uid', '$tem_id', '$fid', '$dir_id', '$fname', now(), now())";
        $result = $mysql->query($sql);
        if ($result) {
            return true;
        } else {
            return false;
        }
    }
}
// �޸��ļ���
function rename_file($id, $new_name)
{
    global $mysql;
    $sql = "update file_dir set filename = '$new_name', update_time = now() where id = '$id'";
    $result = $mysql->query($sql);
    if ($result) {
        return true;
    } else {
        return false;
    }
}
// ɾ���ļ�
function delete_file($id, $fid, $is_last)
{
    global $mysql;
    // ��������һ���ļ���ɾ���ļ�
    if ($is_last) {
        $sql = "delete from files where id = '$fid'";
        $mysql->query($sql);
    }
    else {
        $sql = "update files set count = count - 1 where id = '$fid'";
        $mysql->query($sql);
    }
    // ɾ���ļ����е��ļ�
    $sql = "delete from file_dir where id = '$id'";
    $result = $mysql->query($sql);
    if ($result) {
        return true;
    } else {
        return false;
    }
}
// �����ļ�
function paste_file_by_clip($unique_id, $fname, $dir_id) {
    global $mysql;
    $sql = "update file_dir set did = '$dir_id', filename = '$fname', update_time = now() where id = '$unique_id'";
    $result = $mysql->query($sql);
    if ($result) {
        return true;
    } else {
        return false;
    }
}
// �����ļ�
function paste_file_by_copy($uid, $fid, $fname, $dir_id) {
    global $mysql;
    $tem_id = get_next_id('file_dir');
    $sql = "insert into file_dir(uid, id, fid, did, filename, create_time, update_time) values ('$uid' ,'$tem_id', '$fid', '$dir_id', '$fname', now(), now())";
    $result = $mysql->query($sql);
    if ($result) {
        $sql = "update files set count = count + 1 where id = '$fid'";
        $mysql->query($sql);
        return true;
    } else {
        return false;
    }
}
