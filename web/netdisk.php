<?php session_start();

    header('Content-type:text/html;charset=gbk');

    // 如果还没有登录，则跳转到登录页面
    if (!isset($_SESSION['is_login']) || $_SESSION['is_login'] != 1) {
        // 提示用户未登录
        echo "<script>alert('请先登录！');</script>";
        echo "<script>window.location.href='index.php';</script>";
        exit();
    }

    // 引入connect.php文件
    require_once 'connect.php';

    // 处理新建文件：在uid用户的upath下新建filename的文件
    function handle_new_file($fname, $md5, $addfile)
    {
        $savepath = "/home/netdisk/files/";
        $dir_id = $_SESSION['dir_id'];

        if (($_FILES["file"]["error"] > 0 && $_FILES["file"]["error"] != 4) || (!$md5 && $_FILES["file"]["error"] > 0)) {
            switch ($_FILES["file"]["error"]) {
                case 1:
                    echo "<script>alert('上传的文件超过了php.ini中upload_max_filesize选项限制的值');</script>";
                    break;
                case 2:
                    echo "<script>alert('上传文件的大小超过了HTML表单中MAX_FILE_SIZE选项指定的值');</script>";
                    break;
                case 3:
                    echo "<script>alert('文件只有部分被上传');</script>";
                    break;
                case 4:
                    echo '<script>alert("请选择文件！");</script>';
                    break;
                case 6:
                    echo "<script>alert('找不到临时文件夹');</script>";
                    break;
                case 7:
                    echo "<script>alert('文件写入失败');</script>";
                    break;
            }
        } else {
            // 判断是否存在同名文件
            if (is_same_name($_SESSION['dir_id'], $fname)) {
                echo "<script>alert('文件名已存在！');</script>";
            } else {
                // 处理文件信息
                if ($addfile != "miao") {
                    if (!move_uploaded_file($_FILES["file"]["tmp_name"], $savepath . $md5)) {
                        echo "<script>alert('文件移动失败！');</script>";
                        return;
                    }
                }
                // 处理数据库信息
                if ($addfile == "miao") {
                    // 秒传：文件大小不重要
                    if (insert_file($_SESSION['uid'], $md5, $dir_id, $fname, 0, $addfile)) {
                        echo "<script>alert('文件秒传成功！');</script>";
                    } else {
                        echo "<script>alert('文件秒传失败！');</script>";
                    }
                }
                else {
                    // 不是秒传：需要文件大小
                    if (insert_file($_SESSION['uid'], $md5, $dir_id, $fname, $_FILES["file"]["size"], $addfile)) {
                        echo "<script>alert('文件上传成功！');</script>";
                    } else {
                        echo "<script>alert('文件上传失败！');</script>";
                    }
                }
            }
        }
    }

    // 处理删除文件：在uid用户的upath下删除filename的文件
    function handle_delete_file($id)
    {
        $savepath = "/home/netdisk/files/";

        $dir_id = $_SESSION['dir_id'];
        $fid = get_fid_by_unique_id($id);
        $md5 = get_md5_by_fid($fid);
        if ($fid == -1) {
            echo "<script>alert('文件不存在！');</script>";
            return;
        }
        else {
            $count = get_file_count($fid);
            $is_last = 0;
            if ($count == 1) {
                // 删除文件
                unlink($savepath . $md5);
                $is_last = 1;
                if ($fid == $_SESSION['copyed_id']) {
                    $_SESSION['copyed_id'] = -1;
                    $_SESSION['copyed_name'] = "";
                    $_SESSION['is_clip'] = 0;
                }
            }
            // 删除数据库信息
            if (delete_file($id, $fid, $is_last)) {
                echo "<script>alert('文件删除成功！');</script>";
            } else {
                echo "<script>alert('文件删除失败！');</script>";
            }
        }
    }

    // 判断POST请求
    if (isset($_POST['goto_dir'])) { // 跳转到某个目录
        // 获取目录名
        $dir_name = $_POST['path'];
        $_SESSION['path'] = $dir_name;
        $_SESSION['dir_id'] = get_dir_id($_SESSION['uid'], $dir_name);
    }
    else if (isset($_POST['paste'])) { // 将copyed_id粘贴到当前path下（复制或者剪切）
        // 获取粘贴的文件id
        $copyed_id = $_SESSION['copyed_id'];
        $copyed_name = $_POST['paste_fname'];
        if (is_same_name($_SESSION['dir_id'], $copyed_name)) {
            echo "<script>alert('文件名已存在！');</script>";
        }
        else {
        // 如果是剪切
            if ($_SESSION['is_clip']) {
                if (paste_file_by_clip($copyed_id, $copyed_name, $_SESSION['dir_id'])) {
                    echo "<script>alert('文件剪切成功！');</script>";
                    $_SESSION['is_clip'] = 0;
                    $_SESSION['copyed_id'] = -1;
                    $_SESSION['copyed_name'] = "";
                } else {
                    echo "<script>alert('文件剪切失败！');</script>";
                }
            }
            else {
                if (paste_file_by_copy($_SESSION['uid'], $copyed_id, $copyed_name, $_SESSION['dir_id'])) {
                    echo "<script>alert('文件复制成功！');</script>";
                } else {
                    echo "<script>alert('文件复制失败！');</script>";
                }
            }
        }
    }
    else if (isset($_POST['rename'])) { // 重命名（unique_id）
        // 获取unique_id
        $unique_id = $_POST['id'];
        // 获取新名称
        $new_name = $_POST['new_fname'];

        if (is_same_name($_SESSION['dir_id'], $new_name)) {
            echo "<script>alert('文件名已存在！');</script>";
        } else {
            if (rename_file($unique_id, $new_name)) {
                echo "<script>alert('重命名成功！');</script>";
            } else {
                echo "<script>alert('重命名失败！');</script>";
            }
        }
    }
    else if (isset($_POST['copyed'])) { // 复制（fid）
        // 获取fid
        $fid = $_POST['copyed_id'];
        // 获取名字
        $fname = $_POST['copyed_name'];
        // 设置复制状态
        $_SESSION['is_clip'] = 0;
        // 设置复制的文件id
        $_SESSION['copyed_id'] = $fid;
        // 设置复制的文件名
        $_SESSION['copyed_name'] = $fname;

        echo "<script>alert('复制文件：$fname');</script>";
    }
    else if (isset($_POST['clip'])) { // 剪切（unique_id）
        // 获取unique_id
        $unique_id = $_POST['clip_id'];
        // 获取名字
        $fname = $_POST['clip_name'];
        // 设置剪切状态
        $_SESSION['is_clip'] = 1;
        // 设置剪切的文件id
        $_SESSION['copyed_id'] = $unique_id;
        // 设置剪切的文件名
        $_SESSION['copyed_name'] = $fname;

        echo "<script>alert('剪切文件：$fname');</script>";
    }
    else if (isset($_POST['delfile'])) { // 删除（unique_id）
        // 获取unique_id
        $unique_id = $_POST['delfile_id'];
        if ($unique_id == $_SESSION['copyed_id'] && $_SESSION['is_clip']) {
            $_SESSION['copyed_id'] = -1;
            $_SESSION['copyed_name'] = "";
            $_SESSION['is_clip'] = 0;
        }
        handle_delete_file($unique_id);
    }
    else if (isset($_POST['add_dir_name'])) { // 创建新目录
        // 获取目录名
        $dir_name = $_POST['add_dir_name'];
        if (is_same_name($_SESSION['dir_id'], $dir_name)) {
            echo "<script>alert('目录名已存在！');</script>";
        }
        else {
            // 创建目录
            $tem_id = get_next_id("dirs");
            insert_dir($tem_id, $_SESSION['uid'], $_SESSION['dir_id'], $dir_name);
            echo "<script>alert('创建目录：${dir_name}成功');</script>";
        }
    }
    else if (isset($_POST['addfile'])) { // 创建新文件
        // 获取文件名
        $file_name = $_POST['fname'];
        // 获取md5值
        $md5 = $_POST['md5'];
        // 获取addfile
        $addfile = $_POST['addfile'];

        handle_new_file($file_name, $md5, $addfile);
    }

    // 设置中国时区
    date_default_timezone_set("PRC");

    // 获取文件列表
    $dir_list = get_dir_list($_SESSION['dir_id']);
    $file_list = get_file_list($_SESSION['dir_id']);
    // 引入netdisk.html文件
    require_once './view/netdisk.html';
    
    //关闭数据库
    $mysql->close();
?>