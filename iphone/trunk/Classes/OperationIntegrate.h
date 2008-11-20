//
//  OperationIntegrate.h
//  mathomatic
//
//  Created by Ben Gotow on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Operation.h"

@interface OperationIntegrate : Operation <UIActionSheetDelegate>{

}

- (void)run;
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex;

@end
