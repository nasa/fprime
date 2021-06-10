# Magic Draw Support (Deprecated)

MagicDraw was once used as a design program to augment F´ development.  As of F´ 2.0, it is discontinued as it is
difficult to use and develop in.  The F´ 2.0 series of improvements will being integration of F´´ which will replace
MagicDraw and hand-editing designs as the recommended pattern.

Some users may wish to use magic draw and this document tries to capture some aspects of its use so such users are not
completely unable to get started. **Note:** further MagicDraw use is at the user's own risk and support may not be
available as the OpenSource team does not use nor support this work.

## MagicDraw Notes

F´ has a plugin to MagicDraw that can be used to generate Ai.xml files to use with the autocoder. It is available in the
`Autocoders/MagicDrawCompPlugin` folder of F´.  It can be installed by following the README file there, although support
past MagicDraw 18.5 is not guaranteed.

F´ has some MagicDraw support for existing components, however; given the disuse of these features, not all components
or examples will work.  Open up the `Ref/Top/REFApplication.mdxml` file to begin. If import errors arise, the user will
need to fix them, and that is outside the scope of this document.

Should problems arise when looking for "ISF-MD-Profile.mdzip" it is now located here: `Autocoders/MagicDrawCompPlugin/model`.

At the time of writing this document, Ai.xml files generated with the MagicDraw plugin work correctly with the Autocoder
and there are no plans in place to change this interoperability.  Ongoing support for the plugin and supplied design
files have been discontinued.
