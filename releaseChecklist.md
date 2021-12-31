Release checklist:

# "test" phase
Confirm code changes all work together.

## Automated build succeeds on all platforms
This library is intended to work with many microcontrollers.
Your change should work for all of them. If not, document what variants to expect.

## Unit tests pass
Have you added tests to confirm new features do not regress?

## Usage notes upated for new behaviors
Does **README.md** need to describe new behavior?
Any other documents need an upate?

# "next" phase
Prepare for release

## Updated version number in **library.properties** file
Is this a major release?

## Updated **releaseNotes.md**
What is the reason for this release?

# "release" phase
Make the release visible to others

## Code merged/squashed to **main**
Clones of this library check out **main** by default.

## Commit tagged with version number
Arduino libraries only pick up tagged releases with a matching **library.properties** version, AND in the commit history for the **main** branch.

## Confirm the new release is visible
### It should be visible in the Arduino IDE.
### It should be visible in the PlatformIO IDE.
