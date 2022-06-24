{
  'target_defaults': {
    'conditions': [
      ['OS!="win"', {
        'defines': [
          '_DARWIN_USE_64_BIT_INODE=1',
          '_LARGEFILE_SOURCE',
          '_FILE_OFFSET_BITS=64',
          '_GNU_SOURCE'
        ]
      }],
      [ 'OS=="aix"', {
        'include_dirs': [ 'config/aix' ],
        'sources': [ 'config/aix/ares_config.h' ],
        'defines': [
          # Support for malloc(0)
          '_LINUX_SOURCE_COMPAT=1',
          '_ALL_SOURCE=1'],
      }],
      ['OS=="solaris"', {
        'defines': [
          '__EXTENSIONS__',
          '_XOPEN_SOURCE=500'
        ]
      }]
    ]
  },

  'targets': [
    {
      'target_name': 'cares',
      'type': '<(library)',
      'include_dirs': [ 'include' ],
      'direct_dependent_settings': {
        'include_dirs': [ 'include' ]
      },
      'sources': [
        'include/ares.h',
        'include/ares_dns.h',
        'include/ares_nameser.h',
        'include/ares_rules.h',
        'include/ares_version.h',
        'src/lib/ares__addrinfo2hostent.c',
        'src/lib/ares__addrinfo_localhost.c',
        'src/lib/ares_android.c',
        'src/lib/ares_cancel.c',
        'src/lib/ares__close_sockets.c',
        'src/lib/ares_create_query.c',
        'src/lib/ares_data.c',
        'src/lib/ares_data.h',
        'src/lib/ares_destroy.c',
        'src/lib/ares_expand_name.c',
        'src/lib/ares_expand_string.c',
        'src/lib/ares_fds.c',
        'src/lib/ares_free_hostent.c',
        'src/lib/ares_free_string.c',
        'src/lib/ares_freeaddrinfo.c',
        'src/lib/ares_getenv.h',
        'src/lib/ares_getaddrinfo.c',
        'src/lib/ares_gethostbyaddr.c',
        'src/lib/ares_gethostbyname.c',
        'src/lib/ares__get_hostent.c',
        'src/lib/ares_getnameinfo.c',
        'src/lib/ares_getsock.c',
        'src/lib/ares_init.c',
        'src/lib/ares_ipv6.h',
        'src/lib/ares_library_init.c',
        'src/lib/ares_library_init.h',
        'src/lib/ares_llist.c',
        'src/lib/ares_llist.h',
        'src/lib/ares_mkquery.c',
        'src/lib/ares_nowarn.c',
        'src/lib/ares_nowarn.h',
        'src/lib/ares_options.c',
        'src/lib/ares__parse_into_addrinfo.c',
        'src/lib/ares_parse_aaaa_reply.c',
        'src/lib/ares_parse_a_reply.c',
        'src/lib/ares_parse_caa_reply.c',
        'src/lib/ares_parse_mx_reply.c',
        'src/lib/ares_parse_naptr_reply.c',
        'src/lib/ares_parse_ns_reply.c',
        'src/lib/ares_parse_ptr_reply.c',
        'src/lib/ares_parse_soa_reply.c',
        'src/lib/ares_parse_srv_reply.c',
        'src/lib/ares_parse_txt_reply.c',
        'src/lib/ares_parse_uri_reply.c',
        'src/lib/ares_platform.h',
        'src/lib/ares_private.h',
        'src/lib/ares_process.c',
        'src/lib/ares_query.c',
        'src/lib/ares__read_line.c',
        'src/lib/ares__readaddrinfo.c',
        'src/lib/ares_search.c',
        'src/lib/ares_send.c',
        'src/lib/ares_setup.h',
        'src/lib/ares__sortaddrinfo.c',
        'src/lib/ares_strcasecmp.c',
        'src/lib/ares_strcasecmp.h',
        'src/lib/ares_strdup.c',
        'src/lib/ares_strdup.h',
        'src/lib/ares_strerror.c',
        'src/lib/ares_strsplit.c',
        'src/lib/ares_timeout.c',
        'src/lib/ares__timeval.c',
        'src/lib/ares_version.c',
        'src/lib/ares_writev.c',
        'src/lib/ares_writev.h',
        'src/lib/bitncmp.c',
        'src/lib/bitncmp.h',
        'src/lib/inet_net_pton.c',
        'src/lib/inet_ntop.c',
        'src/lib/ares_inet_net_pton.h',
        'src/lib/setup_once.h',
        'src/tools/ares_getopt.c',
        'src/tools/ares_getopt.h',
      ],
      'conditions': [
        [ 'library=="static_library"', {
          'defines': [ 'CARES_STATICLIB' ]
        }, {
          'defines': [ 'CARES_BUILDING_LIBRARY' ]
        }],
        [ 'OS=="win"', {
          'defines': [
            'CARES_PULL_WS2TCPIP_H=1',
            '_WINSOCK_DEPRECATED_NO_WARNINGS',
          ],
          'include_dirs': [ 'config/win32' ],
          'sources': [
            'src/lib/config-win32.h',
            'src/lib/windows_port.c',
            'src/lib/ares_getenv.c',
            'src/lib/ares_iphlpapi.h',
            'src/lib/ares_platform.c'
          ],
          'libraries': [
            '-lws2_32.lib',
            '-liphlpapi.lib'
          ],
        }, {
          # Not Windows i.e. POSIX
          'cflags': [
            '-g',
            '-pedantic',
            '-Wall',
            '-Wextra',
            '-Wno-unused-parameter'
          ],
          'defines': [ 'HAVE_CONFIG_H' ],
        }],
        [ 'OS not in "win android"', {
          'cflags': [
            '--std=gnu89'
          ],
        }],
        [ 'OS=="linux"', {
          'include_dirs': [ 'config/linux' ],
          'sources': [ 'config/linux/ares_config.h' ]
        }],
        [ 'OS=="mac"', {
          'include_dirs': [ 'config/darwin' ],
          'sources': [ 'config/darwin/ares_config.h' ]
        }],
        [ 'OS=="freebsd" or OS=="dragonflybsd"', {
          'include_dirs': [ 'config/freebsd' ],
          'sources': [ 'config/freebsd/ares_config.h' ]
        }],
        [ 'OS=="openbsd"', {
          'include_dirs': [ 'config/openbsd' ],
          'sources': [ 'config/openbsd/ares_config.h' ]
        }],
        [ 'OS=="android"', {
          'include_dirs': [ 'config/android' ],
          'sources': [ 'config/android/ares_config.h' ],
        }],
        [ 'OS=="solaris"', {
          'include_dirs': [ 'config/sunos' ],
          'sources': [ 'config/sunos/ares_config.h' ],
          'direct_dependent_settings': {
            'libraries': [
              '-lsocket',
              '-lnsl'
            ]
          }
        }]
      ]
    }
  ]
}
