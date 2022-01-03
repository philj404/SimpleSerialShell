Release checklist:

## "test" phase
Confirm code changes all work together.

- [ ] GitHub's automated build succeeds on all platforms (even platforms you don't have).
  - This library is intended to work with many microcontrollers.  Your change should work for all of them. 
  - If not, document what variants should expect.

- [ ] Unit tests pass.
  - [ ] Have you added tests to confirm new features continue to work (so nobody accidentally breaks your feature)?

- [ ] Usage notes upated for new behaviors
  - [ ] Does **README.md** need to describe new behavior?
  - [ ] Any other documents need an upate?

## "next" phase
Prepare for release

- [ ] Updated version number in **library.properties** file
  - Is this a major release?

- [ ] Updated **releaseNotes.md**
  - What is the reason for this release?
  - Is it obvious whether a developer needs this update, or can safely ignore it?

## "release" phase
Make the release visible to others

- [ ] Code merged/squashed to **main**
  - Clones of this library check out **main** by default.

- [ ] Commit tagged with the new version number
  - Arduino libraries only pick up tagged releases with a matching **library.properties** version, AND in the commit history for the **main** branch.

- [ ] Confirm the new release is visible
  - (It may take a day or so for the library managers to discover the new release)
  - [ ] It should be visible in the Arduino IDE.
  - Eventually it should be visible in the PlatformIO IDE. (hopefully this update is automatic)
