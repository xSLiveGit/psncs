<?php 

$cipher = "aes-128-gcm";

function Encrypt($plaintext, $key, $iv, $tag)
{
    $ciphertext = openssl_encrypt($Text, $GLOBALS['cipher'], $GLOBALS['key'], $options=0, $GLOBALS['iv'], $tag);
    return openssl_encrypt($plaintext, $cipher, $GLOBALS['key'], $options=0, $iv, $tag);
}

function Decrypt($Text, $key, $iv, $tag)
{
    return openssl_decrypt($Text, $GLOBALS['cipher'], $GLOBALS['key'], $options=0, $GLOBALS['iv'], $Tag);
}

?>