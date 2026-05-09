{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  pname = "xylarjava";
  version = "0.0.0.4";

  src = ../../publish/linux-x64;

  installPhase = ''
    mkdir -p $out/opt/xylarjava
    cp -R $src/. $out/opt/xylarjava/
    mkdir -p $out/bin
    cat > $out/bin/xylarjava <<EOF
    #!/usr/bin/env bash
    cd "$out/opt/xylarjava"
    exec ./XylarJavaLauncher "$@"
    EOF
    chmod +x $out/bin/xylarjava
  '';
}
