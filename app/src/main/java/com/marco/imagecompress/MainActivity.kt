package com.marco.imagecompress

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.marco.imagecompress.databinding.ActivityMainBinding
import com.zhaodong.compress.ImageCompress

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val compress = ImageCompress()
        binding.sampleText.text = compress.stringFromJNI()
    }
}