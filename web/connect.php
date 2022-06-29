<?php
// 数据库信息
$db_host = "localhost"; //主机
$db_username = "root"; //数据库用户名
$db_password = "root123"; //数据库密码
$db_name = "netdisk";
$mysql = new mysqli($db_host, $db_username, $db_password, $db_name);
if ($mysql->connect_error) {
    // 如果链接失败输出错误，并退出
    die("can't connect" . $mysql->connection_error);
    exit();
} else {
    $result = $mysql->query("set names gbk");
}

// 判断信息
// 判断用户是否存在
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
// 判断文件夹是否已经存在：uid用户的pid文件夹下是否有dir_name
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
// 判断文件是否已经存在：did文件夹下是否有filename
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
// 判断是否有同名
function is_same_name($dir_id, $name)
{
    if (!is_dir_exist($dir_id, $name) && !is_file_exist($dir_id, $name)) {
        return false;
    } else {
        return true;
    }
}
// 判断用户密码是否正确
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

// 获取信息
// 获取下一个id
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
// 获取指定用户的id（用户id从1开始，返回-1表示不存在）
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
// 根据path获取当前目录id（文件id从1开始，返回-1表示不存在）
function get_dir_id($uid, $path)
{
    global $mysql;
    $path = trim($path, "/");
    $path = explode("/", $path);
    $dir_id = 0;
    // 每个用户根目录的pid为0，从用户根目录开始查询
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
// 获取指定md5的文件id（文件id从1开始，返回-1表示不存在）
function get_file_id_by_md5($md5)
{
    global $mysql;
    $sql = "select id from files where md5 = '$md5'";
    $result = $mysql->query($sql);
    // 如果查询结果为空，则返回-1
    if ($result->num_rows == 0) {
        return -1;
    }
    // 如果查询结果不为空，则返回文件id
    else {
        $row = $result->fetch_assoc();
        return $row['id'];
    }
}
// 获取指定fid的md5
function get_md5_by_fid($fid)
{
    global $mysql;
    $sql = "select md5 from files where id = '$fid'";
    $result = $mysql->query($sql);
    $row = $result->fetch_assoc();
    return $row['md5'];
}
// 根据did、filename获取文件id（文件id从1开始，返回-1表示不存在）
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
// 获取unique_id对应的fid
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
// 获取fid对应文件的count
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
// 获取指定用户的文件夹列表（返回数组，每个元素是一个文件夹的信息）
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
// 获取指定用户的文件列表（返回数组，每个元素是一个文件的信息）
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


// users        (id, username, password, root（根目录的名字）);
// files        (id, size, md5, path（文件实际存储位置）, count);

// dirs         (id, uid, pid, name（文件夹名字）, create_time, update_time);
// file_dir    (id, fid, did, uid, filename（文件名）, create_time, update_time);


// 操作函数
// 插入新用户（密码插入为md5值）
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
// 插入文件夹
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
// 插入文件
function insert_file($uid, $md5, $dir_id, $fname, $size, $addfile)
{
    $savepath = "/home/netdisk/files/";
    global $mysql;
    if ($addfile != 'miao') {
        // 没有该文件，插入文件
        $fid = get_next_id('files');
        $sql = "insert into files(id, size, md5, path, count) values ('$fid', '$size', '$md5', '" . $savepath . $md5 . "', '1')";
        $mysql->query($sql);

        // 插入文件到文件夹中
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
        // 有该文件，更新文件信息
        $fid = get_file_id_by_md5($md5);
        $sql = "update files set count = count + 1 where id = '$fid'";
        $mysql->query($sql);

        // 插入文件到文件夹中
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
// 修改文件名
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
// 删除文件
function delete_file($id, $fid, $is_last)
{
    global $mysql;
    // 如果是最后一个文件，删除文件
    if ($is_last) {
        $sql = "delete from files where id = '$fid'";
        $mysql->query($sql);
    }
    else {
        $sql = "update files set count = count - 1 where id = '$fid'";
        $mysql->query($sql);
    }
    // 删除文件夹中的文件
    $sql = "delete from file_dir where id = '$id'";
    $result = $mysql->query($sql);
    if ($result) {
        return true;
    } else {
        return false;
    }
}
// 剪切文件
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
// 复制文件
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
