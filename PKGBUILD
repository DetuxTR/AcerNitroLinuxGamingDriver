_pkgbase="acer_nitro_gaming_driver2"
pkgname="acer_nitro_gaming_driver2-dkms"
pkgver=1
pkgrel=1
pkgdesc="Acer Nitro Gaming Functions Driver for Linux"
arch=('x86_64')
depends=('dkms')
source=("v1.0.0.tar.gz")
package(){
	# Copy dkms.conf
  install -Dm644 dkms.conf "${pkgdir}"/usr/src/${_pkgbase}-${pkgver}/dkms.conf

  # Set name and version
  sed -e "s/@_PKGBASE@/${_pkgbase}/" \
      -e "s/@PKGVER@/${pkgver}/" \
      -i "${pkgdir}"/usr/src/${_pkgbase}-${pkgver}/dkms.conf

  # Copy sources (including Makefile)
  cp -r ./* "${pkgdir}"/usr/src/${_pkgbase}-${pkgver}/

}
