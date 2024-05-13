SUMARY = "Smart Kitchen demo"
DESCRIPTION = "Recipe of Smart Kitchen demo application"
SECTION = "Multimedia"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=242ef8a3d67a79b1e67096c5e87de59d"

NXP_SMART_KITCHEN_SRC = "gitsm://bitbucket.sw.nxp.com/mag/smart-kitchen.git;protocol=ssh"
SRCBRANCH = "ffmpeg-gui-test"
DEMODIR = "/home/root/.nxp-demo-experience/scripts/multimedia/smart-kitchen"


SRC_URI = "${NXP_SMART_KITCHEN_SRC};branch=${SRCBRANCH};name=smartkitchen \
			file://0001-Added-custom_tick_get-function.patch"

SRCREV_FORMAT = "smartkitchen"
SRCREV_smartkitchen = "${AUTOREV}"

S = "${WORKDIR}/git"

DEMOS ?= ""

DEPENDS = "wayland libxkbcommon libxdg-shell wayland-protocols xdg-utils libav ffmpeg"


PACKAGECONFIG = "avdevice avfilter avcodec avformat swresample swscale postproc"

# libraries to build in addition to avutil
PACKAGECONFIG[avdevice] = "--enable-avdevice,--disable-avdevice"
PACKAGECONFIG[avfilter] = "--enable-avfilter,--disable-avfilter"
PACKAGECONFIG[avcodec] = "--enable-avcodec,--disable-avcodec"
PACKAGECONFIG[avformat] = "--enable-avformat,--disable-avformat"
PACKAGECONFIG[swresample] = "--enable-swresample,--disable-swresample"
PACKAGECONFIG[swscale] = "--enable-swscale,--disable-swscale"
PACKAGECONFIG[postproc] = "--enable-postproc,--disable-postproc"

RDEPENDS:${PN}+= " bash demo-experience-imx-voiceui-smart-kitchen python3-posix-ipc libxdg-shell wayland-protocols xdg-utils ffmpeg"

do_patch() {
	mv ${WORKDIR}/0001-Added-custom_tick_get-function.patch ${WORKDIR}/git/lvgl
	cd ${WORKDIR}/git/lvgl/ && git am 0001-Added-custom_tick_get-function.patch
	cd ${WORKDIR}/git/
	cp -r wayland-client/* lv_drivers/wayland/
}

do_compile() {
	cd ${S}
	make
}

do_install() {
    install -d -m 755 ${D}/home/root/.nxp-demo-experience/scripts/multimedia/smart-kitchen
    cp -r ${S}/smart-kitchen-deploy/* ${D}/home/root/.nxp-demo-experience/scripts/multimedia/smart-kitchen
}

FILES:${PN} += "/home/root/.nxp-demo-experience/scripts/multimedia/smart-kitchen"

TARGET_CC_ARCH += "${LDFLAGS}"
