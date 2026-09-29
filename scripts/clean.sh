ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

BUILD_DIR="$ROOT_DIR/build"
IMAGE_NAME="ipv4-stack"


case "$(uname -s)" in
	Linux)
		cmake --build "$BUILD_DIR" --target clean
		;;
	Darwin) # MacOS
		docker image rm "$IMAGE_NAME"
		;;
	*)  # Might change later, need to check if docker runs linux container on all systesm
		echo "Unsupported operating system">&2
		exit 1
		;;
	
esac