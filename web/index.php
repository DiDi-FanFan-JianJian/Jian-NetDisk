<?php session_start();
    /*
     * 登录注册页面
     * 如果是get请求，则返回index.html页面
     * 如果是post请求，则判断请求结果
     *    如果登录成功，则弹框提示，并跳转到index.php页面
     *    如果登录失败，则弹框提示错误信息，并返回index.php页面
     * 
     *    如果注册成功，则弹框提示注册成功，并返回index.php页面
     *    如果注册失败，则弹框提示错误信息，并返回index.php页面
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
    // 判断请求类型
    if ($_SERVER['REQUEST_METHOD'] == 'GET') {
        include './view/index.html';
    } else if ($_SERVER['REQUEST_METHOD'] == 'POST') {
        $username = $_POST['username'];
        $password = $_POST['password'];

        if ($username && $password) { // 如果用户名和密码都不为空
            // 建立数据库连接
            include './connect.php';

            if (isset($_POST["signup"])) { // 判断注册
                if (is_user_exist($username)) { // 用户名已存在
                    echo "<script>alert('用户名已存在，3秒后跳转回登录/注册页面');</script>";
                    echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                } else { // 用户名不存在
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
                        echo "<script>alert('密码必须包含大写字母、小写字母、数字、特殊字符中的三种，且长度不小于12，3秒后跳转回登录/注册页面');</script>";
                        echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                    } else {
                        // 数据库插入新用户及根目录
                        $user_id = get_next_id("users");
                        $root_id = get_next_id('dirs');
                        insert_user($user_id, $username, $password, $root_id);
                        insert_dir($root_id, $user_id, 0, 'root');
    
                        echo "<script>alert('注册成功，3秒后跳转回登录/注册页面');</script>";
                        echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                    }
                }
            } else { // 判断登录
                if (!is_user_exist($username)) { // 用户名不存在
                    echo "<script>alert('用户名不存在，3秒后跳转回登录/注册页面');</script>";
                    echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                }
                else if (check_password($username, $password)) {// 登录成功
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
                    // 跳转到netdisk.php页面
                    echo "<script>alert('登录成功，3秒后跳转回网盘页面');</script>";
                    header('Location: netdisk.php');
                } else { // 登录失败
                    echo "<script>alert('密码错误，3秒后跳转回登录/注册页面');</script>";
                    echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
                }
            }

            // 关闭数据库连接
            $mysql->close();
        } else { // 如果用户名或密码为空
            echo "表单填写不完整，3秒后跳转回登录/注册页面";
            echo "<script>setTimeout(function(){window.location.href='index.php';},3000);</script>";
        }
    }
?>
