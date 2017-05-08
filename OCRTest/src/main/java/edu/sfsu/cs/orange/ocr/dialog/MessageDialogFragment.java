package edu.sfsu.cs.orange.ocr.dialog;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;

/**
 * Created by khurram on 04/05/2017.
 */

public class MessageDialogFragment extends DialogFragment {
public interface Listener {
    void onPositiveButtonTapped(MessageDialogFragment  messageDialogFragment);
    void onNegativeButtonTapped(MessageDialogFragment  messageDialogFragment);
}
@NonNull
@Override
public Dialog onCreateDialog(Bundle savedInstanceState) {

    return new AlertDialog.Builder(getContext())
            .setTitle(title)
            .setMessage(message)
            .setPositiveButton(positiveButtonTitle, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {

                }
            })
            .setNegativeButton(negativeButtonTitle, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {

                }
            })
            .create();
}

public void setPositiveButtonTitle(String positiveButtonTitle) { this.positiveButtonTitle = positiveButtonTitle; }
public void setNegativeButtonTitle(String negativeButtonTitle) { this.negativeButtonTitle = negativeButtonTitle; }
public void setTitle(String title) { this.title = title; }
public void setMessage(String message) { this.message = message; }
public void setListener(Listener listener) { this.listener = listener; }

private String positiveButtonTitle;
private String negativeButtonTitle;
private String title;
private String message;
private Listener listener;
}
