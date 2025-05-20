#include "shared_fd.h"
#include "shared_select.h"

#include <stdlib.h>
#include <unistd.h>
#include <gtest/gtest.h>

#include <string>

namespace redroid {

char pipe_message[] = "Testing the pipe";

TEST(SendFD, Basic) {
  SharedFD fds[2];
  SharedFD::Pipe(fds, fds + 1);
  EXPECT_TRUE(fds[0]->IsOpen());
  EXPECT_TRUE(fds[1]->IsOpen());
  EXPECT_EQ(sizeof(pipe_message), fds[1]->Write(pipe_message, sizeof(pipe_message)));
  char buf[80];
  EXPECT_EQ(sizeof(pipe_message), fds[0]->Read(buf, sizeof(buf)));
  EXPECT_EQ(0, strcmp(buf, pipe_message));
}

}
