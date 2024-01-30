package com.example.irobotapplication;

import android.annotation.SuppressLint;
import android.content.pm.ActivityInfo;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatImageButton;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

public class ControllerFragment extends Fragment {

    private ConnectiviyCheck check;
    private ActionBar actionBar;

    public ControllerFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_controller, container, false);
    }

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        // Nasconde la toolbar nell'activity associata al fragment
        AppCompatActivity activity = (AppCompatActivity) getActivity();
        if (activity != null) {
            actionBar = activity.getSupportActionBar();
            if (actionBar != null) {
                actionBar.hide();
            }
        }

        requireActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        AppCompatImageButton buttonForward = requireView().findViewById(R.id.bntUp);
        AppCompatImageButton buttonBackwards = requireView().findViewById(R.id.btnDown);
        AppCompatImageButton buttonRotSx = requireView().findViewById(R.id.btnRotSX);
        AppCompatImageButton buttonRotDx = requireView().findViewById(R.id.btnRotDX);

        check = new ConnectiviyCheck(requireActivity().getApplicationContext());
        check.startPeriodicRequests();

        //using this boolean to prevent multiple buttons from being pressed at the same time
        AtomicBoolean anyButtonPressed = new AtomicBoolean(false);

        buttonForward.setOnTouchListener((v, event) -> {
            if (event.getAction() == MotionEvent.ACTION_UP && anyButtonPressed.get()) {
                // when button is released
                anyButtonPressed.set(false);
                postToServer("btnReleased");
                buttonForward.setImageResource(R.drawable.vettoreup_bianco__removebg_preview);
                return true;
            } else if (event.getAction() == MotionEvent.ACTION_DOWN && !anyButtonPressed.get()) {
                // when button is pressed
                anyButtonPressed.set(true);
                postToServer("up");
                buttonForward.setImageResource((R.drawable.vettoreup_rosso__removebg_preview));
                return true;
            }
            return false;
        });

        buttonBackwards.setOnTouchListener((v, event) -> {
            if (event.getAction() == MotionEvent.ACTION_UP && anyButtonPressed.get()) {
                // when button is released
                anyButtonPressed.set(false);
                postToServer("btnReleased");
                buttonBackwards.setImageResource(R.drawable.vettoredown_bianco__removebg_preview);
                return true;
            } else if (event.getAction() == MotionEvent.ACTION_DOWN && !anyButtonPressed.get()) {
                // when button is pressed
                anyButtonPressed.set(true);
                postToServer("down");
                buttonBackwards.setImageResource((R.drawable.vettoredown_rosso__removebg_preview));
                return true;
            }
            return false;
        });

        buttonRotSx.setOnTouchListener((v, event) -> {
            if (event.getAction() == MotionEvent.ACTION_UP && anyButtonPressed.get()) {
                // when button is released
                anyButtonPressed.set(false);
                postToServer("btnReleased");
                buttonRotSx.setImageResource(R.drawable.rotsx_bianco_2);
                return true;
            } else if (event.getAction() == MotionEvent.ACTION_DOWN && !anyButtonPressed.get()) {
                // when button is pressed
                anyButtonPressed.set(true);
                postToServer("left");
                buttonRotSx.setImageResource(R.drawable.rotsx_rosso__removebg_preview);
                return true;
            }
            return false;
        });

        buttonRotDx.setOnTouchListener((v, event) -> {
            if (event.getAction() == MotionEvent.ACTION_UP && anyButtonPressed.get()) {
                // when button is released
                anyButtonPressed.set(false);
                postToServer("btnReleased");
                buttonRotDx.setImageResource(R.drawable.rotdx_bianco);
                return true;
            } else if (event.getAction() == MotionEvent.ACTION_DOWN && !anyButtonPressed.get()) {
                // when button is pressed
                anyButtonPressed.set(true);
                postToServer("right");
                buttonRotDx.setImageResource(R.drawable.rotdx_rosso__removebg_preview);
                return true;
            }
            return false;
        });
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();

        // Ripristina la toolbar quando il fragment viene distrutto
        if (actionBar != null) {
            actionBar.show();
        }

        check.stopPeriodicRequests();
        postToServer("commStop");
    }

    private void postToServer(String direction) {

        RequestQueue queue = Volley.newRequestQueue(requireContext());

        StringRequest stringRequest = new StringRequest(Request.Method.POST, getString(R.string.conn_string) + "/control",
                response -> Log.d("HTTP-POST", "Response: " + response),
                error -> {
                    // Handle errors here.
                    Log.e("HTTP-POST", "Error: " + error.toString());
                })
        {
            @Override
            public byte[] getBody() {
                // define the POST parameters as JSON.
                JSONObject jsonBody = new JSONObject();
                try {
                    jsonBody.put("direction", direction);
                } catch (JSONException e) {
                    Log.e("JSON in POST", "JSON Error: " + e.getMessage());
                }
                return jsonBody.toString().getBytes(StandardCharsets.UTF_8);
            }

            @Override
            public Map<String, String> getHeaders() {
                // set headers, including Content-Type.
                Map<String, String> headers = new HashMap<>();
                headers.put("Content-Type", "application/json");
                return headers;
            }

            @Override
            public String getBodyContentType() {
                return "application/json";
            }
        };
        queue.add(stringRequest);
    }
}