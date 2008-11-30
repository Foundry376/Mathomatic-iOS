//
//  EquationDivisionView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "EquationContainerView.h"

@interface EquationTwoPartView : EquationContainerView {

}

- (void)finalizeEquationTree:(BOOL)clean;
- (BOOL)isTreeLegal;

@end
