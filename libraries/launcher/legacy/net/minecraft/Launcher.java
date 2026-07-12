package net.minecraft;

import java.applet.Applet;
import java.applet.AppletStub;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Graphics;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

/**
 * WARNING: This class is reflectively accessed by legacy Forge versions.
 * <p>
 * Changing field and method declarations without further testing is not
 * recommended.
 */
public final class Launcher extends Applet implements AppletStub {
    private static final long serialVersionUID = 1L;

    private final Map<String, String> params = new HashMap<>();

    private Applet wrappedApplet;
    private final URL documentBase;
    private boolean active = false;

    public Launcher(Applet applet) {
        this(applet, null);
    }

    public Launcher(Applet applet, URL documentBase) {
        setLayout(new BorderLayout());

        add(applet, "Center");

        wrappedApplet = applet;

        try {
            if (documentBase == null) {
                if (applet.getClass().getPackage().getName().startsWith("com.mojang"))
                    // Special case only for Classic versions
                    documentBase = new URL("http://www.minecraft.net:80/game/");
                else
                    documentBase = new URL("http://www.minecraft.net/game/");
            }
        } catch (MalformedURLException e) {
            throw new AssertionError(e);
        }

        this.documentBase = documentBase;
    }

    public void replace(Applet applet) {
        wrappedApplet = applet;

        applet.setStub(this);
        applet.setSize(getWidth(), getHeight());

        setLayout(new BorderLayout());
        add(applet, "Center");

        applet.init();

        active = true;

        applet.start();

        validate();
    }

    @Override
    public boolean isActive() {
        return active;
    }

    @Override
    public URL getDocumentBase() {
        return documentBase;
    }

    @Override
    public URL getCodeBase() {
        try {
            return new URL("http://www.minecraft.net/game/");
        } catch (MalformedURLException e) {
            throw new AssertionError(e);
        }
    }

    @Override
    public String getParameter(String key) {
        String param = params.get(key);

        if (param != null)
            return param;

        try {
            return super.getParameter(key);
        } catch (Throwable ignored) {
        }

        return null;
    }

    @Override
    public void resize(int width, int height) {
        wrappedApplet.resize(width, height);
    }

    @Override
    public void resize(Dimension size) {
        wrappedApplet.resize(size);
    }

    @Override
    public void init() {
        if (wrappedApplet != null)
            wrappedApplet.init();
    }

    @Override
    public void start() {
        wrappedApplet.start();

        active = true;
    }

    @Override
    public void stop() {
        wrappedApplet.stop();

        active = false;
    }

    @Override
    public void destroy() {
        wrappedApplet.destroy();
    }

    @Override
    public void appletResize(int width, int height) {
        wrappedApplet.resize(width, height);
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);

        wrappedApplet.setVisible(visible);
    }

    @Override
    public void paint(Graphics graphics) {}

    @Override
    public void update(Graphics graphics) {}

    public void setParameter(String key, String value) {
        params.put(key, value);
    }

    public void setParameter(String key, boolean value) {
        setParameter(key, value ? "true" : "false");
    }
}
