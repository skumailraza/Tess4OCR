package edu.sfsu.cs.orange.ocr;

import android.content.Context;
import android.hardware.usb.UsbManager;
import android.widget.Toast;

import tw.com.prolific.driver.pl2303.PL2303Driver;


public class KBWedge
{
    private PL2303Driver mSerial;
    UsbManager manager;
    Context mContext;
    String sAppName;

    public String StatusMessage = new String();

    public void ClearStatus()
    {
        StatusMessage = "";
    }

    private void SetStatus(String msg)
    {
        StatusMessage = "KBWedge: " + msg;
        Toast.makeText(mContext, StatusMessage, Toast.LENGTH_SHORT).show();
    }

    public KBWedge(UsbManager manager, Context mContext, String sAppName)
    {
        this.manager = manager;
        this.mContext = mContext;
        this.sAppName = sAppName;

        ClearStatus();
        mSerial = new PL2303Driver(manager,mContext,sAppName);

        if(mSerial==null)
        {
            SetStatus("Could not create driver instance.");
            return;
        }
    }

    public boolean Open()
    {
        int res = 0;

        ClearStatus();

        if(mSerial==null)
        {
            SetStatus("Could not create driver instance.");
            return false;
        }

        onResume(); // call enumerate here in case mSerial was not created on first Resume

        // check USB host function.
        if (!mSerial.PL2303USBFeatureSupported())
        {
            SetStatus("USB host API Not Supported.");
            return false;
        }

        if(!mSerial.isConnected())
        {
            SetStatus("Serial chip not connected.");
            return false;
        }

        if(!mSerial.InitByPortSetting(PL2303Driver.BaudRate.B9600, PL2303Driver.DataBits.D8, PL2303Driver.StopBits.S1, PL2303Driver.Parity.NONE, PL2303Driver.FlowControl.OFF))
        {
            if(!mSerial.PL2303Device_IsHasPermission())
                SetStatus("Cannot open device, possibly no permission");
            else if(!mSerial.PL2303Device_IsSupportChip())
                SetStatus("Cannot open device, possibly this chip has no support");
            else
                SetStatus("Open failed");

            return false;
        }

        mSerial.PL2303Device_SetCommTimeouts(700); // 700ms timeout

        SetStatus("Open OK");

        return true;
    }

    public void Close()
    {
        ClearStatus();
        if(mSerial != null)
        {
            mSerial.end();
            SetStatus("Closed");
        }
    }

    public void Destroy()
    {
        ClearStatus();
        if(mSerial != null)
        {
            Close();
            mSerial.end();
            mSerial = null;
            SetStatus("Destroyed");
        }
    }

    public void onResume()
    {
        ClearStatus();
        if(mSerial == null)
            return;

        if(!mSerial.isConnected())
        {
            if( !mSerial.enumerate() ) // enumerate is important because it sets up the broadcast notify listener for USB attach
            {
                SetStatus("device not found");
                return;
            }
        }
        SetStatus("device attached");

    }
    public int Write(String data)
    {
        ClearStatus();
        String msg = new String();
        if(mSerial==null || !mSerial.isConnected())
        {
            SetStatus("Write: not connected");
            return -1;
        }
        byte[] bytes = data.getBytes();
        int len = data.length();
        String.format(msg, "Writing %d chars...", len);
        SetStatus(msg);

        int nwrote = mSerial.write(bytes, len);
        String.format(msg, "Wrote %d chars", nwrote);
        SetStatus(msg);
        return nwrote;
    }

    public int Read(StringBuilder data) // loopback testing
    {
        ClearStatus();
        String msg = new String();
        if(mSerial==null || !mSerial.isConnected())
        {
            SetStatus("Read: not connected");
            return -1;
        }
        int len = 0;
        byte[] rbuf = new byte[256];
        len = mSerial.read(rbuf);
        if(len>0)
        {
            for(int i=0;i<len;i++)
                data.append((char)(rbuf[i]&0x000000FF));
        }
        String.format(msg, "Read %d chars", len);
        SetStatus(msg);

        return len;
    }
}
