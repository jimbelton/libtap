PORTNAME=	libtap
PORTVERSION=	2.01
CATEGORIES=	devel
MASTER_SITES=	# none
DISTFILES=	# none

DISTDIR=	${MASTERDIR}/../output
PACKAGES=	${MASTERDIR}/../output
PKGREPOSITORYSUBDIR=	.
NO_WRKSUBDIR=	yes
NO_EXTRACT=	yes
NO_BUILD=	yes

MAINTAINER=	cie-eng.dns-platform@cisco.com
COMMENT=	Library implementing the Test Anything Protocol

do-install:
	cp ../tap.h ${STAGEDIR}${PREFIX}/include/.
	cp ../target/libtap.a ${STAGEDIR}${PREFIX}/lib/.
	cp ../tap.3 ${STAGEDIR}${PREFIX}/man/man3/.

.include <bsd.port.mk>
