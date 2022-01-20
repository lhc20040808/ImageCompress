package com.marco.imagecompress

import android.Manifest
import android.content.Context
import android.content.Intent
import android.database.Cursor
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Bundle
import android.os.Environment
import android.provider.MediaStore
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.marco.imagecompress.databinding.ActivityMainBinding
import com.zhaodong.compress.ImageCompress
import java.io.File
import java.io.IOException
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {

    private val IMAGE_REQUEST_CODE = 101

    private lateinit var binding: ActivityMainBinding
    private val singleThreadPool = Executors.newSingleThreadExecutor()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        ActivityCompat.requestPermissions(this,
            arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE),
            0)

        val compress = ImageCompress()
        binding.sampleText.text = compress.stringFromJNI()

        binding.tvOpenCamera.setOnClickListener {
            val intent = Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
            intent.type = "image/*"
            startActivityForResult(intent, IMAGE_REQUEST_CODE)
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (data == null) {
            Toast.makeText(this, "获取图片失败", Toast.LENGTH_SHORT).show();
            return
        }
        val uriFromAlbum = data.data!!
        singleThreadPool.submit(Runnable {
            try {
                val picturePath: String = getImagePath(this@MainActivity, uriFromAlbum)
                val orgFile = File(picturePath)
                val index = picturePath.lastIndexOf("/")
                val orgName = picturePath.substring(index + 1)
                val bitmap = BitmapFactory.decodeFile(picturePath)
                val newFilePath =
                    Environment.getExternalStorageDirectory().absolutePath + "/Pictures/Screenshots/${
                        orgName.replace("org",
                            "dst")
                    }"
                val dstFile = File(newFilePath)
                if (dstFile.exists()) {
                    dstFile.delete()
                }
                dstFile.createNewFile()
                val compress = ImageCompress()
                val ret = compress.nativeCompressBitmap(bitmap, 60, newFilePath)
                Log.d("lhc", "压缩结果:$ret newFilePath:${newFilePath}")
                val orgSize = orgFile.length()
                val dstSize = File(newFilePath).length()
                Log.d("lhc", "原文件大小:${orgSize / 1024f} , 现文件大小:${dstSize / 1024f}")
            } catch (e: IOException) {
                e.printStackTrace()
            }
        })

    }

    fun getImagePath(context: Context, uri: Uri): String {
        var cursor: Cursor? = null
        return try {
            val proj = arrayOf(MediaStore.Images.Media.DATA)
            cursor = context.contentResolver.query(uri, proj, null, null, null)
            val column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA)
            cursor.moveToFirst()
            cursor.getString(column_index)
        } finally {
            cursor?.close()
        }
    }
}